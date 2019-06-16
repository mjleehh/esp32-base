#include <mfl/color_display/LvglAdapter.hpp>
#include <lvgl/lvgl.h>

namespace mfl::color_display {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

mfl::ColorDisplay* activeDisplay = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

void flushCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* colorMap) {
    if (activeDisplay != nullptr) {
        activeDisplay->flush(x1, y1, x2, y2, colorMap);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void fillCurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color) {
    if (activeDisplay != nullptr) {
        activeDisplay->fill(x1, y1, x2, y2, color);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}

// ---------------------------------------------------------------------------------------------------------------------

LvglAdapter::LvglAdapter(ColorDisplay& display) {
    if (activeDisplay != nullptr) {
        throw LvglAdapterError("A lvgl display is already active");
    }
    activeDisplay = &display;

    std::function<void()> flushWrapper = lv_flush_ready;
    display.setColorsSentHandler(flushWrapper);
    lv_init();

    static lv_disp_drv_t disp;
    lv_disp_drv_init(&disp);
    disp.disp_flush = flushCurrent;
    disp.disp_fill = fillCurrent;
    lv_disp_drv_register(&disp);
}

// ---------------------------------------------------------------------------------------------------------------------

LvglAdapter::~LvglAdapter() {
    activeDisplay = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

void LvglAdapter::runTasks(uint32_t millis) {
    lv_tick_inc(millis);
    lv_task_handler();
}

// ---------------------------------------------------------------------------------------------------------------------

}
