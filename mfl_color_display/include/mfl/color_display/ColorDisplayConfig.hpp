#pragma once


namespace mfl::color_display {

// ---------------------------------------------------------------------------------------------------------------------

struct ColorDisplayConfig {
    gpio_num_t clockPin           = UNDEFINED_PIN;
    gpio_num_t dataPin            = UNDEFINED_PIN;
    gpio_num_t dataFromDisplayPin = UNDEFINED_PIN;
    gpio_num_t chipSelectPin      = UNDEFINED_PIN;
    gpio_num_t commandPin         = UNDEFINED_PIN;
    gpio_num_t resetPin           = UNDEFINED_PIN;
    gpio_num_t dataReadPin        = UNDEFINED_PIN;
    gpio_num_t backlightPin       = UNDEFINED_PIN;
};

// ---------------------------------------------------------------------------------------------------------------------

}
