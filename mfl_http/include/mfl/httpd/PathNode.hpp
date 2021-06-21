#pragma once

#include <mfl/httpd/Handler.hpp>
#include <mfl/http/Method.hpp>

#include <map>
#include <vector>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct PathNode {
    PathNode(std::vector<std::string>&& args);

    HandlerWrapper& handlerFromMethod(http::Method method);
    const HandlerWrapper& handlerFromMethod(http::Method method) const;

    bool hasHandler(http::Method method) const;
    void setHandler(http::Method method, const HandlerWrapper& handler);

    HandlerWrapper post;
    HandlerWrapper get;
    HandlerWrapper put;
    HandlerWrapper del;
    std::vector<std::string> args;
    std::map<std::string, PathNode> children;
};

// ---------------------------------------------------------------------------------------------------------------------

}
