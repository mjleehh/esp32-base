#include <freertos/portmacro.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <mfl/Wifi.hpp>
#include <mfl/EspHttpd.hpp>
#include <sdkconfig.h>

const char* tag = "main";

extern "C" void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();

    mfl::Wifi wifi(CONFIG_DEVICE_HOSTNAME, CONFIG_WIFI_SSID, CONFIG_WIFI_KEY);
    mfl::httpd::Router router;
    router.get("/message", [](mfl::httpd::Context<std::string>& ctx){
        ctx.res.body = "hello!";
    });
    mfl::EspHttpd httpd(router);

    wifi.start([&httpd](const ip4_addr& addr){
        httpd.start();

        ip4_addr ip = addr;
        uint8_t* bytes = reinterpret_cast<uint8_t*>(&ip);
        ESP_LOGI(tag, "serving at %d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);

    });

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
