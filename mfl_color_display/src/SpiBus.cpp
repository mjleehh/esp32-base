#include <mfl/color_display/SpiBus.hpp>

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <cstring>
#include <esp_log.h>

namespace mfl::color_display {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

// FIXME: quick hack, remove ASAP
SpiBus* currentBus = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

}

// ---------------------------------------------------------------------------------------------------------------------

SpiBus::SpiBus(const SpiBusConfig& config)
    : commandPin_(config.commandPin), transmissionEndHandler_(config.transmissionEndHandler), transferInProgress_(false)
{
    if (currentBus != nullptr) {
        throw SpiBusConfigError("due to a bug in ESP IDF currently there can only be one SPI bus connected");
    }
    currentBus = this;

    if (config.clockPin == UNDEFINED_PIN) {
        throw SpiBusConfigError("clock pin is required in SPI config");
    }

    if (config.dataPin == UNDEFINED_PIN) {
        throw SpiBusConfigError("data pin is required in SPI config");
    }

    if (config.chipSelectPin == UNDEFINED_PIN) {
        throw SpiBusConfigError("chip select pin is required in SPI config");
    }

    if (config.commandPin != UNDEFINED_PIN) {
        gpio_set_direction(config.commandPin, GPIO_MODE_OUTPUT);
    }

    // TODO: add a device abstraction if needed
    spi_device_interface_config_t devcfg = {
            .mode = 0,
            .clock_speed_hz = 40*1000*1000,
            .spics_io_num = config.chipSelectPin,
            .flags = SPI_DEVICE_HALFDUPLEX,
            .queue_size = 1,
            .pre_cb = nullptr,
            .post_cb = transmissionEndIsr,
    };

    // check if we have 3 wire transfer
    if (config.dataReadPin != UNDEFINED_PIN) {
        throw std::invalid_argument("not implemented");
    } else {
        spi_bus_config_t buscfg = {
                .mosi_io_num = config.dataPin,
                .miso_io_num = -1,
                .sclk_io_num = config.clockPin,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
                .max_transfer_sz = config.maxTransfertSize,
        };
        ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, 1));
        ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &devcfg, &spi_));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SpiBus::sendCommand(uint8_t cmd) {
    if (commandPin_ != UNDEFINED_PIN) {
        // command mode
        gpio_set_level(commandPin_, 0);
    }
    sendRaw(&cmd, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpiBus::sendData(const uint8_t *data, uint16_t length) {
    if (commandPin_ != UNDEFINED_PIN) {
        // data mode
        gpio_set_level(commandPin_, 1);
    }
    sendRaw(data, length);
}

void SpiBus::sendRaw(const uint8_t *data, uint16_t length) {
    if (length == 0) {
        return;
    }

    while (transferInProgress_) {
        // waiting
    }

    spi_transaction_t transaction = {
        .length = length * 8, // length in bits!
        .user = this,
        .tx_buffer = data,
    };
    transferInProgress_ = true;
    spi_device_queue_trans(spi_, &transaction, portMAX_DELAY);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpiBus::handleTransmissionEnd() {
    transferInProgress_ = false;
    if (transmissionEndHandler_) {
        transmissionEndHandler_();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void IRAM_ATTR SpiBus::transmissionEndIsr(spi_transaction_t *transaction) {
// FIXME: the user property gets corrupted for some reason
// FIXME: terrible terrible hack
//    volatile auto user = reinterpret_cast<SpiBus*>(transaction->user);
//    if (user != nullptr) {
//        user->handleTransmissionEnd();
//    }
    if (currentBus != nullptr) {
        currentBus->handleTransmissionEnd();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}
