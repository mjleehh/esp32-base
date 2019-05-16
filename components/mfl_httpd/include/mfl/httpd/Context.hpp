#pragma once

#include <mfl/httpd/Response.hpp>

#include <esp_http_server.h>
#include <map>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

using Params = std::map<std::string, std::string>;

// ---------------------------------------------------------------------------------------------------------------------

struct Context {
    Context(Params&& params, std::string&& body, httpd_req_t* handle)
        : params(params), body(body), handle(handle)
    {}

    Params params;
    std::string body;
    httpd_req_t *handle;

    Response res;
};

// ---------------------------------------------------------------------------------------------------------------------

}
