#pragma once

#include <driver/gpio.h>
#include <stdexcept>
#include <driver/spi_master.h>
#include <functional>

namespace mfl::color_display {

// ---------------------------------------------------------------------------------------------------------------------

const gpio_num_t UNDEFINED_PIN = GPIO_NUM_NC;

// ---------------------------------------------------------------------------------------------------------------------

struct SpiBusConfigError : std::invalid_argument {
    explicit SpiBusConfigError(const std::string& what) : std::invalid_argument(what) {}
};

// ---------------------------------------------------------------------------------------------------------------------

struct SpiBusConfig {
    gpio_num_t clockPin          = UNDEFINED_PIN;
    gpio_num_t dataFromMasterPin = UNDEFINED_PIN;
    gpio_num_t dataToMasterPin   = UNDEFINED_PIN;
    gpio_num_t chipSelectPin     = UNDEFINED_PIN;
    gpio_num_t commandPin        = UNDEFINED_PIN;
    gpio_num_t dataReadPin       = UNDEFINED_PIN;
    std::function<void()> transmissionEndHandler = nullptr;
    uint16_t maxTransfertSize = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

struct SpiBus {
    explicit SpiBus(const SpiBusConfig& config);

    void sendCommand(uint8_t cmd);
    void sendData(const uint8_t* data, uint16_t length);

private:
    void sendRaw(const uint8_t* data, uint16_t length);

    spi_device_handle_t spi_;
    gpio_num_t commandPin_;
    std::function<void()> transmissionEndHandler_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
