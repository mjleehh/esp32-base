#pragma once

#include <mfl/httpd/arg-types.hpp>
#include <mfl/httpd/Errors.hpp>

#include <map>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct Params {
    template <typename T>
    T get(const std::string& argName) const {
        try {
            return argtypes::deserialize<T>(values_.at(argName));
        } catch (const std::out_of_range& e) {
            throw EndpointError("no such argument: " + argName);
        }
    }

    void reset(std::map<std::string, std::string>&& values) {
        values_ = values;
    }

private:
    std::map<std::string, std::string> values_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
