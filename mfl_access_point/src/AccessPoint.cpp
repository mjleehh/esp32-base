#include <mfl/AccessPoint.hpp>
#include <esp_wifi.h>
#include <mdns.h>
#include <esp_log.h>

namespace mfl {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "mfl_access_point";

// ---------------------------------------------------------------------------------------------------------------------

AccessPoint* activeAccessPoint = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

}

// ---------------------------------------------------------------------------------------------------------------------

AccessPoint::AccessPoint(const std::string &hostname, const std::string &ssid, const std::string &key)
        : hostname_(hostname), ssid_(ssid), key_(key)
{

}

// ---------------------------------------------------------------------------------------------------------------------

void AccessPoint::addService(const std::string &type, AccessPoint::Protocol protocol, uint16_t port, const std::string &name,
                             const std::map<std::string, std::string> &props) {
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

void AccessPoint::start(const std::function<void()>& onStart) {
    if (activeAccessPoint != nullptr) {
        throw AccessPointError("another access point is currently active");
    }
    activeAccessPoint = this;
    onStart_ = onStart;

    ESP_ERROR_CHECK(esp_event_loop_init(eventHandler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(hostname_.c_str()));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_ap_config_t apConfig {
        .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        .max_connection = 1,
    };
    std::copy(ssid_.begin(), ssid_.end(), apConfig.ssid);
    std::copy(key_.begin(), key_.end(), apConfig.password);
    if (key_.empty()) {
        apConfig.authmode = WIFI_AUTH_OPEN;
    }
    wifi_config_t wifiConfig = {.ap = apConfig};

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t AccessPoint::eventHandler(void *ctx, system_event_t *event) {
    mdns_handle_system_event(ctx, event);

    if (activeAccessPoint == nullptr) {
        return ESP_FAIL;
    }

    if (event->event_id == SYSTEM_EVENT_AP_STACONNECTED) {
        ESP_LOGI(tag, "station join");
    } else if (event->event_id == SYSTEM_EVENT_AP_STADISCONNECTED) {
        ESP_LOGI(tag, "station leave");
    } else if (event->event_id == SYSTEM_EVENT_AP_START) {
        if (activeAccessPoint != nullptr && activeAccessPoint->onStart_) {
            activeAccessPoint->onStart_();
        }
    } else if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {
        ESP_LOGI(tag, "SYSTEM_EVENT_STA_GOT_IP");
    } else if (event->event_id == SYSTEM_EVENT_AP_STAIPASSIGNED) {
        ESP_LOGI(tag, "SYSTEM_EVENT_AP_STAIPASSIGNED");
    } else if (event->event_id == SYSTEM_EVENT_ETH_GOT_IP) {
        ESP_LOGI(tag, "SYSTEM_EVENT_ETH_GOT_IP");
    }

    return ESP_OK;
}

// ---------------------------------------------------------------------------------------------------------------------

}
