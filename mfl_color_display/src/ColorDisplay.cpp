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

ColorDisplay* activeDisplay = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "color display";

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// ---------------------------------------------------------------------------------------------------------------------

ColorDisplay::ColorDisplay(ColorDisplay::ControllerType controllerType, gpio_num_t clockPin, gpio_num_t mosiPin,
                           gpio_num_t deviceSelectPin, gpio_num_t commandPin, gpio_num_t resetPin)
    : commandPin_(commandPin), transferInProgress_(false), colorsSent_(false)
{
    if (activeDisplay != nullptr) {
        ESP_LOGE(tag, "display is a singleton");
    }
    activeDisplay = this;

    spi_bus_config_t buscfg = {
            .mosi_io_num = mosiPin,
            .miso_io_num = -1,
            .sclk_io_num = clockPin,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = LV_VDB_SIZE * 2,
    };

    spi_device_interface_config_t devcfg = {
            .mode = 0,
            .clock_speed_hz = 40*1000*1000,
            .spics_io_num = deviceSelectPin,
            .queue_size = 1,
            .pre_cb = nullptr,
            .post_cb = spiReady,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, 1));
    ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &devcfg, &spi_));

    using namespace mfl_color_display;
    lcd_init_cmd_t ili_init_cmds[]={
            {SWRESET, {}, 0}, // reset
            {SLPOUT, {}, 0}, // wake
            {COLMOD, {0x55}, 1}, // 16 bit color mode
            {MADCTL, {0x28}, 1}, // memory access data control
            {CASET, {0x00, 0x00, 0x00, 0xEF}, 4},
            {RASET, {0x00, 0x00, 0x00, 0xEF}, 4},
            {RAMWR, {0}, 0},
            {SLPOUT, {0}, 0},
            {DISPON, {0}, 0},
            {0, {0}, 0xff},
    };

    gpio_set_direction(commandPin, GPIO_MODE_OUTPUT);
    gpio_set_direction(resetPin, GPIO_MODE_OUTPUT);

    // reset display
    gpio_set_level(resetPin, 0);
    vTaskDelay(100 / portTICK_RATE_MS) ;
    gpio_set_level(resetPin, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    //Send all the commands
    uint16_t cmd = 0;
    while (ili_init_cmds[cmd].databytes!=0xff) {
        sendCommand(ili_init_cmds[cmd].cmd);
        sendData(ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes&0x1F);
        if (ili_init_cmds[cmd].databytes & 0x80) {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        cmd++;
    }

    lv_init();

    static lv_disp_drv_t disp;
    lv_disp_drv_init(&disp);
    disp.disp_flush = flushCurrent;
    disp.disp_fill = fillCurrent;
    lv_disp_drv_register(&disp);

    fill(10, 10, 50, 50, lv_color_t {.red = 0x1f});

}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendRaw(const uint8_t *data, uint16_t length) {
    if (length == 0) {
        return;
    }

    while(transferInProgress_) {}

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = length * 8; // transfer length in bits!
    t.tx_buffer = data;
    transferInProgress_ = true;
    spi_device_queue_trans(spi_, &t, portMAX_DELAY);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendCommand(uint8_t cmd) {
    // command mode
    gpio_set_level(commandPin_, 0);

    colorsSent_ = false;
    sendRaw(&cmd, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendData(const uint8_t* data , uint16_t length) {
    // data mode
    gpio_set_level(commandPin_, 1);

    colorsSent_ = false;
    sendRaw(data, length);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendColors(const lv_color_t* data, uint16_t length) {
    // data mode
    gpio_set_level(commandPin_, 1);

    colorsSent_ = true;
    auto dataPtr = reinterpret_cast<const uint8_t*>(data);
    sendRaw(dataPtr, sizeof(lv_color_t) * length);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap) {
    uint8_t data[4];

    using namespace mfl_color_display;

    /*Column addresses*/
    sendCommand(CASET);
    data[0] = (x1 >> 8) & 0xFF;
    data[1] = x1 & 0xFF;
    data[2] = (x2 >> 8) & 0xFF;
    data[3] = x2 & 0xFF;
    sendData(data, 4);

    /*Page addresses*/
    sendCommand(RASET);
    data[0] = (y1 >> 8) & 0xFF;
    data[1] = y1 & 0xFF;
    data[2] = (y2 >> 8) & 0xFF;
    data[3] = y2 & 0xFF;
    sendData(data, 4);

    /*Memory write*/
    sendCommand(RAMWR);

    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);

    // send data
    sendColors(colorMap, size * 2);

//	lv_flush_ready();
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color) {
    uint8_t data[4];

    using namespace mfl_color_display;

    /*Column addresses*/
    sendCommand(CASET);
    data[0] = (x1 >> 8) & 0xFF;
    data[1] = x1 & 0xFF;
    data[2] = (x2 >> 8) & 0xFF;
    data[3] = x2 & 0xFF;
    sendData(data, 4);

    /*Page addresses*/
    sendCommand(RASET);
    data[0] = (y1 >> 8) & 0xFF;
    data[1] = y1 & 0xFF;
    data[2] = (y2 >> 8) & 0xFF;
    data[3] = y2 & 0xFF;
    sendData(data, 4);

    /*Memory write*/
    sendCommand(RAMWR);

    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);
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

void ColorDisplay::fillCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color) {
    if (activeDisplay != nullptr) {
        activeDisplay->fillCurrent(x1, y1, x2, y2, color);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void IRAM_ATTR ColorDisplay::spiReady(spi_transaction_t*) {
    if (activeDisplay != nullptr) {
        activeDisplay->transferInProgress_ = false;
        if (activeDisplay->colorsSent_) {
            lv_flush_ready();
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl

