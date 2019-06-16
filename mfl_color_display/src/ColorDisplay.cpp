#include "mfl/ColorDisplay.hpp"

#include "ST7735-commands.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/spi_master.h>
#include <lv_conf.h>
#include <esp_log.h>
#include <lvgl/lvgl.h>


namespace mfl {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

ColorDisplay* activeDisplay = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
    uint16_t delay;
} lcd_init_cmd_t;

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "color display";

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// ---------------------------------------------------------------------------------------------------------------------

ColorDisplay::ColorDisplay(const color_display::ColorDisplayConfig& config)
    : spi_(createBusConfig(config))
{
    if (activeDisplay != nullptr) {
        throw ColorDisplayError("A lvgl display is already active");
    }
    activeDisplay = this;

    using namespace color_display;
    lcd_init_cmd_t ili_init_cmds[]={
            {SWRESET, {},    0, 200}, // reset
            {SLPOUT, {},     0, 500}, // wake
            {COLMOD, {0x55}, 1, 50}, // 16 bit color mode
            {MADCTL, {MADCTL_MX | MADCTL_MY},    1, 50}, // memory access data control
            {CASET, {0x00, 0x00, 0x00, 0xEF}, 4, 0},
            {RASET, {0x00, 0x00, 0x00, 0xEF}, 4, 0},
            //{RAMWR, {}, 0},
            {NORON, {}, 0, 200},
            {DISPON, {}, 0, 200},
            {0, {}, 0xff, 0},
    };

    gpio_set_direction(config.resetPin, GPIO_MODE_OUTPUT);

    // reset display
    gpio_set_level(config.resetPin, 0);
    vTaskDelay(100 / portTICK_RATE_MS) ;
    gpio_set_level(config.resetPin, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    // send setup sequence
    uint16_t cmd = 0;
    while (ili_init_cmds[cmd].databytes != 0xff) {
        sendCommand(ili_init_cmds[cmd].cmd);
        sendData(ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes);
        auto delay = ili_init_cmds[cmd].delay;
        if (delay > 0) {
            vTaskDelay(delay / portTICK_RATE_MS);
        }
        cmd++;
    }

    lv_init();

    static lv_disp_drv_t disp;
    lv_disp_drv_init(&disp);
    disp.disp_flush = flushCurrent;
    disp.disp_fill = fillCurrent;
    lv_disp_drv_register(&disp);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendCommand(uint8_t cmd) {
    colorsSent_ = false;
    spi_.sendCommand(cmd);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendData(const uint8_t* data , uint16_t length) {
    colorsSent_ = true;
    spi_.sendData(data, length);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendColors(const lv_color_t* data, uint16_t length) {
    colorsSent_ = true;
    auto dataPtr = reinterpret_cast<const uint8_t*>(data);
    spi_.sendData(dataPtr, sizeof(lv_color_t) * length);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap) {
    setDrawWindow(x1, y1, x2, y2);

    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);
    sendCommand(color_display::RAMWR);
    sendColors(colorMap, size);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color) {
    setDrawWindow(x1, y1, x2, y2);

    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);
    sendCommand(color_display::RAMWR);
    lv_color_t buf[LV_HOR_RES];

    uint32_t i;
    if(size < LV_HOR_RES) {
        for(i = 0; i < size; i++) buf[i] = color;
    } else {
        for(i = 0; i < LV_HOR_RES; i++) buf[i] = color;
    }

    while(size > LV_HOR_RES) {
        sendColors(buf, LV_HOR_RES);
        size -= LV_HOR_RES;
    }
    sendColors(buf, size);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::flushCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap) {
    if (activeDisplay != nullptr) {
        activeDisplay->flush(x1, y1, x2, y2, colorMap);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::setDrawWindow(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    uint8_t data[4];

    // column address
    sendCommand(color_display::CASET);
    data[0] = (x1 >> 8) & 0xFF;
    data[1] = x1 & 0xFF;
    data[2] = (x2 >> 8) & 0xFF;
    data[3] = x2 & 0xFF;
    sendData(data, 4);

    // page address
    sendCommand(color_display::RASET);
    data[0] = (y1 >> 8) & 0xFF;
    data[1] = y1 & 0xFF;
    data[2] = (y2 >> 8) & 0xFF;
    data[3] = y2 & 0xFF;
    sendData(data, 4);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::fillCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color) {
    if (activeDisplay != nullptr) {
        activeDisplay->fillCurrent(x1, y1, x2, y2, color);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

color_display::SpiBusConfig ColorDisplay::createBusConfig(const color_display::ColorDisplayConfig &config) {
    return color_display::SpiBusConfig {
            .clockPin = config.clockPin,
            .dataPin  = config.dataPin,
            .chipSelectPin = config.chipSelectPin,
            .commandPin = config.commandPin,
            .dataReadPin = config.dataReadPin,
            .transmissionEndHandler = handleEndTransmission,
            .maxTransfertSize = LV_VDB_SIZE * 4,
    };
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::handleEndTransmission() {
    if(activeDisplay->colorsSent_) {
//        lv_flush_ready();
    };
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl

