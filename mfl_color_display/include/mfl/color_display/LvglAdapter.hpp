#pragma once

#include <mfl/ColorDisplay.hpp>

namespace mfl::color_display {

// ---------------------------------------------------------------------------------------------------------------------

struct LvglAdapterError : std::logic_error {
    explicit LvglAdapterError(const std::string& what) : std::logic_error(what) {}
};

// ---------------------------------------------------------------------------------------------------------------------

struct LvglAdapter {
    explicit LvglAdapter(ColorDisplay& display);
    ~LvglAdapter();

    void runTasks(uint32_t millis);
};

// ---------------------------------------------------------------------------------------------------------------------

}
