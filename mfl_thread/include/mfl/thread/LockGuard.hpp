#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <stdexcept>

namespace mfl::thread {

// ---------------------------------------------------------------------------------------------------------------------

struct LockTimeout : std::runtime_error {
    explicit LockTimeout() : std::runtime_error("failed to acquire lock") {};
};

// ---------------------------------------------------------------------------------------------------------------------

struct ReleaseUnownedLock : std::runtime_error {
    explicit ReleaseUnownedLock() : std::runtime_error("tried to release unowned lock") {};
};

// ---------------------------------------------------------------------------------------------------------------------

struct LockGuard {
    LockGuard(SemaphoreHandle_t handle, uint millis = 10000u): handle_(nullptr) {
        auto res = xSemaphoreTake(handle, 10000);
        if(!res) {
            throw LockTimeout();
        }
        handle_ = handle;
    }

    ~LockGuard() {
        if (handle_ != nullptr) {
            xSemaphoreGive(handle_);
        }
    }

    void release() {
        if (handle_ == nullptr) {
            throw ReleaseUnownedLock();
        }
        xSemaphoreGive(handle_);
        handle_ = nullptr;
    }

private:
    SemaphoreHandle_t handle_;
};

// ---------------------------------------------------------------------------------------------------------------------

}