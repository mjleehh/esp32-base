#include <freertos/portmacro.h>

#include <mfl/Display.hpp>
#include <sdkconfig.h>

const char* tag = "main";

template<int pin>
constexpr gpio_num_t pinFromInt() {
    static_assert(pin >= GPIO_NUM_0, "pin number can not be nagative");
    static_assert(pin < GPIO_NUM_MAX, "pin number is larger than available pins");
    return (gpio_num_t) pin;
}

extern "C" void app_main() {
    mfl::Display display(mfl::Display::ControllerType::ssd1306,
                         pinFromInt<CONFIG_DISPLAY_CLOCK_PIN>(),
                         pinFromInt<CONFIG_DISPLAY_DATA_PIN>(),
                         pinFromInt<CONFIG_DISPLAY_RESET_PIN>());

    for (int i = 20; true; ++i) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        auto h = display.handle();
        u8g2_ClearBuffer(h);
        u8g2_SetFont(h, u8g2_font_profont12_mf);
        u8g2_DrawStr(h, 30, 35, "hello world!");
        u8g2_DrawCircle(h, i, 50, 10, U8G2_DRAW_ALL);
        u8g2_SendBuffer(h);
        if (i >= 108) {
            i = 20;
        }
    }
}
