#include "ST7735_controller.hpp"
#include "ST7735-commands.hpp"
#include "Command.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace mfl::color_display {

// ---------------------------------------------------------------------------------------------------------------------

void init_ST7735_controller(ColorDisplay::Internal& display) {
    Command initCommands[]={
            {SWRESET, {},    0, 200}, // reset
            {SLPOUT, {},     0, 500}, // wake
            {COLMOD, {0x55}, 1, 50}, // 16 bit color mode
            {MADCTL, {MADCTL_MX | MADCTL_MY},    1, 50}, // memory access data control
            {CASET, {0x00, 0x00, 0x00, 0xEF}, 4, 0},
            {RASET, {0x00, 0x00, 0x00, 0xEF}, 4, 0},
            //{RAMWR, {}, 0},
            {NORON, {}, 0, 200},
            {DISPON, {}, 0, 200},
            {0, {}, 0xff, 0},
    };

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
}

// ---------------------------------------------------------------------------------------------------------------------

}