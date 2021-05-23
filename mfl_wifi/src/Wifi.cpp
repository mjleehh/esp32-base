#include <mfl/Wifi.hpp>

#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_event_loop.h>

#include <mdns.h>
#include <vector>
#include <esp_event.h>


namespace mfl {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "mfl_wifi";

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// ---------------------------------------------------------------------------------------------------------------------

Wifi* Wifi::activeWifi = 0;

// ---------------------------------------------------------------------------------------------------------------------

Wifi::Wifi(const std::string& hostname, const std::string& ssid, const std::string& key, const Bssid& bssid)
        : hostname_(hostname), ssid_(ssid), key_(key), bssid_(bssid)
{

}

// ---------------------------------------------------------------------------------------------------------------------

void Wifi::eventHandler(void*, esp_event_base_t eventBase, int32_t eventId, void* eventData) {
    if (activeWifi == nullptr) {
        return;
    }

    if (eventBase == WIFI_EVENT) {
        switch (eventId) {
            case WIFI_EVENT_STA_START:
                ESP_LOGD(tag, "Received a start request");
                ESP_ERROR_CHECK(esp_wifi_connect());
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(tag, "Wifi disconnected");
                if (activeWifi->onFail_) {
                    activeWifi->onFail_();
                    activeWifi = 0;
                }
                break;
            default:
                ESP_LOGI(tag, "other Wifi Event %i", eventId);
        }
    } else if (eventBase == IP_EVENT) {
        switch (eventId) {
            case IP_EVENT_STA_GOT_IP: {
                auto ip = reinterpret_cast<ip_event_got_ip_t *>(eventData)->ip_info.ip;
                ESP_LOGI(tag, "Got an IP: " IPSTR, IP2STR(&ip));
                if (activeWifi->onIp_) {
                    ESP_LOGD(tag, "Calling startup handler");
                    activeWifi->onIp_(ip);
                }
                break;
            }
            default:
                ESP_LOGI(tag, "other IP Event %i", eventId);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Wifi::addService(const std::string& type, Protocol protocol, uint16_t port, const std::string& name, const std::map<std::string, std::string>& props) {
    const char* protoStr = protocol == Protocol::tcp ? "_tcp" : "_udp";
    std::vector<mdns_txt_item_t> propVector;
    for (const auto& elem: props) {
        propVector.push_back(mdns_txt_item_t{
                const_cast<char*>(elem.first.c_str()),
                const_cast<char*>(elem.second.c_str())
        });
    }
    const char* namePtr = name.empty() ? 0 : name.c_str();

    ESP_ERROR_CHECK(mdns_service_add(namePtr, type.c_str(),protoStr, port, propVector.data(), propVector.size()));
}

// ---------------------------------------------------------------------------------------------------------------------

void Wifi::start(const IpHandler& onIp, const FailHandler& onFail) {
    // this is not thread safe
    if (activeWifi != 0) {
        throw WifiError("another WIFI is currently active");
    }
    activeWifi = this;
    onIp_ = onIp;
    onFail_ = onFail;

    //s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &eventHandler,
                                                        NULL,
                                                        nullptr));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &eventHandler,
                                                        NULL,
                                                        nullptr));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(hostname_.c_str()));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_sta_config_t staConfig = {};
    std::copy(ssid_.begin(), ssid_.end(), staConfig.ssid);
    std::copy(key_.begin(), key_.end(), staConfig.password);
    if (bssid_.size() == 6) {
        ESP_LOGI(tag, "BSSID IS NOT EMPTY %i", bssid_.size());
        std::copy(bssid_.begin(), bssid_.end(), staConfig.bssid);

        staConfig.bssid_set = true;
    } else {
        ESP_LOGI(tag, "BSSID IS EMPTY %i", bssid_.size());
        staConfig.bssid_set = false;
    }

    wifi_config_t wifiConfig = {.sta = staConfig};
    for (auto val : wifiConfig.sta.bssid) {
        ESP_LOGI(tag, "value %i", val);
    }
    ESP_LOGI(tag, "is on %i", wifiConfig.sta.bssid_set);

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl

