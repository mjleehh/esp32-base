#pragma once

#include <driver/gpio.h>
#include <lvgl/lv_misc/lv_color.h>
#include <driver/spi_master.h>
#include <mfl/color_display/SpiBus.hpp>
#include <mfl/color_display/ColorDisplayConfig.hpp>
#include <memory>
#include <mutex>

namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

struct ColorDisplay {
    enum class Controller {
        st7725,
        ili9341,
    };

    explicit ColorDisplay(const color_display::ColorDisplayConfig& config);

    void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap);
    void fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);

    void setColorsSentHandler(const std::function<void()>& handler);

    struct Internal;
    friend struct Internal;

private:
    void setDrawWindow(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    color_display::SpiBusConfig createBusConfig(const color_display::ColorDisplayConfig& config);

    void sendCommand(uint8_t cmd);
    void sendData(const uint8_t* data, uint16_t length);
    void sendColors(const lv_color_t* data, uint16_t length);

    std::function<void()> colorsSentHandler_;
    std::unique_ptr<color_display::SpiBus> spi_;
    volatile bool colorsSent_;
    std::mutex lock_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
