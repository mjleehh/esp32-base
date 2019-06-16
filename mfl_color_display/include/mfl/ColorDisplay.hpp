#pragma once

#include <driver/gpio.h>
#include <lvgl/lv_misc/lv_color.h>
#include <driver/spi_master.h>
#include <mfl/color_display/SpiBus.hpp>
#include <mfl/color_display/ColorDisplayConfig.hpp>

namespace mfl {

struct ColorDisplayError : std::logic_error {
    explicit ColorDisplayError(const std::string& what) : std::logic_error(what) {}
};

// ---------------------------------------------------------------------------------------------------------------------

struct ColorDisplay {
    ColorDisplay(const color_display::ColorDisplayConfig& spiBusConfig);

    void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap);
    void fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
private:
    static void flushCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap);
    static void fillCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);

    void setDrawWindow(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    static color_display::SpiBusConfig createBusConfig(const color_display::ColorDisplayConfig& config);
    static void handleEndTransmission();

    void sendCommand(uint8_t cmd);
    void sendData(const uint8_t* data, uint16_t length);
    void sendColors(const lv_color_t* data, uint16_t length);

    color_display::SpiBus spi_;
    volatile bool colorsSent_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
