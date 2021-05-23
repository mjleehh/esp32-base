#pragma once

#include <mfl/httpd/arg-types.hpp>

#include <map>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct Params {
    template <typename T>
    T get(const std::string& argName) const {
        return argtypes::deserialize<T>(values_.at(argName));
    }

    void reset(std::map<std::string, std::string>&& values) {
        values_ = values;
    }

private:
    std::map<std::string, std::string> values_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
