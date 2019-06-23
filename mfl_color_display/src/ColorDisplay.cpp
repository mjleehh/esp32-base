#include "mfl/ColorDisplay.hpp"
#include "ColorDisplay_Internal.hpp"
#include "ST7735-commands.hpp"
#include "ILI9341_controller.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>


namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

ColorDisplay::ColorDisplay(const color_display::ColorDisplayConfig& config) {
    spi_ = std::make_unique<color_display::SpiBus>(createBusConfig(config));

    gpio_set_direction(config.resetPin, GPIO_MODE_OUTPUT);

    // reset display
    gpio_set_level(config.resetPin, 0);
    vTaskDelay(100 / portTICK_RATE_MS) ;
    gpio_set_level(config.resetPin, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    Internal internal(*this);
    color_display::init_ILI9341_controller(internal);

    if (config.backlightPin != color_display::UNDEFINED_PIN) {
        // hack hack
        auto level = config.backlightPinInverted ^ 1;
        ESP_LOGI(TAG, "turning on display at %i", config.backlightPin);
        gpio_set_direction(config.backlightPin, GPIO_MODE_OUTPUT);
        gpio_set_level(config.backlightPin, level);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendCommand(uint8_t cmd) {
    colorsSent_ = false;
    spi_->sendCommand(cmd);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendData(const uint8_t* data , uint16_t length) {
    colorsSent_ = true;
    spi_->sendData(data, length);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::sendColors(const lv_color_t* data, uint16_t length) {
    colorsSent_ = true;
    auto dataPtr = reinterpret_cast<const uint8_t*>(data);
    spi_->sendData(dataPtr, sizeof(lv_color_t) * length);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap) {
    lock_.lock();
    setDrawWindow(x1, y1, x2, y2);

    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);
    sendCommand(color_display::RAMWR);
    sendColors(colorMap, size);
    lock_.unlock();
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color) {
    lock_.lock();
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
    lock_.unlock();
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

color_display::SpiBusConfig ColorDisplay::createBusConfig(const color_display::ColorDisplayConfig &config) {
    auto colorSentWrapper = [this]{
        if (colorsSent_ && colorsSentHandler_) {
            colorsSentHandler_();
        }
    };

    return color_display::SpiBusConfig {
            .clockPin = config.clockPin,
            .dataFromMasterPin = config.dataPin,
            .dataToMasterPin   = config.dataFromDisplayPin,
            .chipSelectPin = config.chipSelectPin,
            .commandPin = config.commandPin,
            .dataReadPin = config.dataReadPin,
            .transmissionEndHandler = colorSentWrapper,
            .maxTransfertSize = LV_VDB_SIZE * 4,
    };
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::setColorsSentHandler(const std::function<void()>& handler) {
    colorsSentHandler_ = handler;
}

// ---------------------------------------------------------------------------------------------------------------------

}
