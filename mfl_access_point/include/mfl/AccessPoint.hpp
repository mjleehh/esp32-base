#pragma once

#include <string>
#include <functional>
#include <map>
#include <esp_event_loop.h>

namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

struct AccessPointError : std::logic_error {
    explicit AccessPointError(const std::string& what) : std::logic_error(what) {}
};

// ---------------------------------------------------------------------------------------------------------------------

struct AccessPoint {
    enum class Protocol {
        tcp,
        udp,
    };

    AccessPoint(const std::string& hostname, const std::string& ssid, const std::string& key);

    void start(const std::function<void()>& onStart);

    void addService(const std::string& type, Protocol protocol, uint16_t port,
                    const std::string& name = "",
                    const std::map<std::string, std::string>& props = {});

private:
    static esp_err_t eventHandler(void *ctx, system_event_t *event);

    const std::string hostname_;
    const std::string ssid_;
    const std::string key_;
    std::function<void()> onStart_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
