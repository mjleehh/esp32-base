#pragma once

#include <mfl/ColorDisplay.hpp>

namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

struct ColorDisplay::Internal {
    Internal(ColorDisplay &colorDisplay);

    void sendCommand(uint8_t cmd);
    void sendData(const uint8_t* data, uint16_t length);

private:
    ColorDisplay& colorDisplay_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
