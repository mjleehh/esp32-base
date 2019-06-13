#pragma once

#include <driver/gpio.h>
#include <lvgl/lv_misc/lv_color.h>
#include <driver/spi_master.h>

namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * 4 wire SPI color displays
 */
struct ColorDisplay {
    enum class ControllerType {
        st7735
    };

    static const gpio_num_t UNDEFINED_PIN = GPIO_NUM_MAX;

    /**
     *
     * @param controllerType
     * @param clockPin        SCL
     * @param mosiPin         SDA
     * @param deviceSelectPin CS (CSX)
     * @param commandPin      RS (DCX)
     * @param resetPin        RST
     */
    ColorDisplay(ControllerType controllerType,
            gpio_num_t clockPin,
            gpio_num_t mosiPin,
            gpio_num_t deviceSelectPin,
            gpio_num_t commandPin,
            gpio_num_t resetPin);

private:
    static void spiReady(spi_transaction_t*);
    static void flushCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap);
    static void fillCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
    void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap);
    void fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);

    void sendCommand(uint8_t cmd);
    void sendData(const uint8_t* data, uint16_t length);
    void sendColors(const lv_color_t* data, uint16_t length);
    void sendRaw(const uint8_t* data, uint16_t length);

    spi_device_handle_t spi_;
    gpio_num_t commandPin_;
    volatile bool transferInProgress_;
    volatile bool colorsSent_;


};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
