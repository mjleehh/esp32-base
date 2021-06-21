#pragma once

#include <map>

namespace mfl::httpd {

enum class Status {
    ok                  = 200,
    badRequest          = 400,
    notFound            = 404,
    internalServerError = 500,
};

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
struct Response {
    Response() : status(Status::ok) {}

    Status status;
    T body;
};

// ---------------------------------------------------------------------------------------------------------------------

struct WrapperResponse {
    WrapperResponse() : status(Status::ok), returnType("text/plain") {}

    Status status;
    std::string returnType;
    std::string body;
};

// ---------------------------------------------------------------------------------------------------------------------

}
