
#include <freertos/portmacro.h>
#include <mfl/AccessPoint.hpp>
#include <esp_log.h>

const char* tag = "main";

extern "C" void app_main() {
    mfl::AccessPoint accessPoint("myAP", "esp32testAP", "verySecret");
    accessPoint.start([]{
        ESP_LOGI(tag, "acces point started");
    });

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}