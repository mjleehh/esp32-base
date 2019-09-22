#include <freertos/portmacro.h>
#include <esp_log.h>

#include <mfl/Wifi.hpp>
#include <sdkconfig.h>

const char* tag = "main";

extern "C" void app_main() {
    mfl::Wifi wifi(CONFIG_DEVICE_HOSTNAME, CONFIG_WIFI_SSID, CONFIG_WIFI_KEY);
    wifi.start([](const ip4_addr& addr){
        ip4_addr ip = addr;
        uint8_t* bytes = reinterpret_cast<uint8_t*>(&ip);
        ESP_LOGI(tag, "received IP %d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    });

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}