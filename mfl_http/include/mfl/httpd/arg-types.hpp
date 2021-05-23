#pragma once

#include <cstdlib>

namespace mfl::httpd::argtypes {

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
T deserialize(const std::string& reqBody);

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string deserialize(const std::string& param) {
    return param;
}

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline int deserialize(const std::string& param) {
    return std::stoi(param);
}

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline uint deserialize(const std::string& param) {
    return std::stoul(param);
}

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline float deserialize(const std::string& param) {
    return std::stof(param);
}

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline double deserialize(const std::string& param) {
    return std::stod(param);
}

// ---------------------------------------------------------------------------------------------------------------------

}