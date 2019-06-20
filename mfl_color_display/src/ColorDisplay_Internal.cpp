#include "ColorDisplay_Internal.hpp"

namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

ColorDisplay::Internal::Internal(ColorDisplay &colorDisplay) : colorDisplay_(colorDisplay) {

}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::Internal::sendCommand(uint8_t cmd) {
    colorDisplay_.sendCommand(cmd);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorDisplay::Internal::sendData(const uint8_t *data, uint16_t length) {
    colorDisplay_.sendData(data, length);
}

// ---------------------------------------------------------------------------------------------------------------------

}
