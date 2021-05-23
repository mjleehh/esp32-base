#pragma once

namespace mfl::http::response_types {

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
inline std::string getMimeType();

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string getMimeType<std::string>() {
    return "text/plain";
}

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
std::string serialize(T& resBody);

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string serialize(const std::string& resBody) {
    return resBody;
}

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string serialize(std::string& resBody) {
    return resBody;
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * this is the placeholder for void output type (handlers with no result)
 */
template<>
inline std::string serialize(nullptr_t&) {
    return "";
}

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
T deserialize(const std::string& reqBody);

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string deserialize(const std::string& reqBody) {
    return reqBody;
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * this is the placeholder for void input type (handlers with no input arguments)
 */
template<>
inline nullptr_t deserialize(const std::string& reqBody) {
    return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

}