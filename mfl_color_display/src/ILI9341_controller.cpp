#include "ILI9341_controller.hpp"
#include "ST7735-commands.hpp"
#include "Command.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

namespace mfl::color_display {

// ---------------------------------------------------------------------------------------------------------------------

void init_ILI9341_controller(ColorDisplay::Internal& display) {
    Command initCommands[]= {
        {SWRESET, {}, 0, 200},
        {POWERA, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5, 0},
        {POWERB, {0x00, 0XC1, 0X30}, 3, 0},
        {DTCA, {0x84, 0x11, 0x7a}, 3, 0},
        {DTCB, {0x66, 0x00}, 2, 0},
        {POWER_SEQ, {0x55, 0x01, 0x23, 0x01}, 4, 0},
        {PRC, {0x10}, 1, 0},
        {PWCTR1, {0x09}, 1, 0},
        {PWCTR2, {0x10}, 1, 0},
        {VMCTR1, {0x3e, 0x28}, 2, 0},
        {VMCTR2, {0x86}, 1, 0},
        {MADCTL, {MADCTL_MX | MADCTL_MY},    1, 50},
        {COLMOD, {0x55}, 1, 0},
        {INVOFF, {}, 0, 0},
        {FRMCTR1, {0x00, 0x1b}, 2, 0},
        {DISCTRL, {0x0a, 0x82, 0x27, 0x00}, 4, 0},
        {PTLAR, {0x00, 0x00, 0x01, 0x3f}, 4, 0},
        {SLPOUT, {}, 0, 120},
        {DISPON, {}, 0, 200},
        {0, {}, 0xff, 0},
    };


    ESP_LOGI(TAG, "running ILI9341 startup sequence");
// send setup sequence
    uint16_t cmd = 0;
    while (initCommands[cmd].databytes != 0xff) {
        display.sendCommand(initCommands[cmd].cmd);
        display.sendData(initCommands[cmd].data, initCommands[cmd].databytes);
        auto delay = initCommands[cmd].delay;
        if (delay > 0) {
            vTaskDelay(delay / portTICK_RATE_MS);
        }
        cmd++;
    }
    ESP_LOGI(TAG, "started ILI9341");
}

// ---------------------------------------------------------------------------------------------------------------------

}