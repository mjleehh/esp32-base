#pragma once

#include <stdexcept>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

const uint16_t DEFAULT_PORT = 80;
const size_t DEFAULT_BUFFER_SIZE = 3 * 4096;
const size_t DEFAULT_STACK_SIZE = 3 * 4096;

// ---------------------------------------------------------------------------------------------------------------------

struct EndpointError : std::invalid_argument {
    explicit EndpointError(const std::string& what) : invalid_argument(what) {};
};

// ---------------------------------------------------------------------------------------------------------------------

}