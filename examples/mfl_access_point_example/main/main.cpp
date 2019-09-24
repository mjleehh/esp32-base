#include <freertos/portmacro.h>
#include <mfl/AccessPoint.hpp>
#include <esp_log.h>
#include <nvs_flash.h>

#include <sdkconfig.h>

const char* tag = "main";

extern "C" void app_main() {
    nvs_flash_init();
    tcpip_adapter_init();

    mfl::AccessPoint accessPoint(CONFIG_DEVICE_HOSTNAME, CONFIG_AP_NAME, CONFIG_AP_KEY);
    accessPoint.start([]{
        ESP_LOGI(tag, "acces point started");
    });

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}