#pragma once

#include <mfl/httpd/Response.hpp>
#include <mfl/http/Method.hpp>

#include <map>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

using Params = std::map<std::string, std::string>;

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
struct replaceVoidWithPlaceholder {
    using type = T;
};

template<>
struct replaceVoidWithPlaceholder<void> {
    using type = nullptr_t ;
};

// ---------------------------------------------------------------------------------------------------------------------

template<typename InT, typename OutT = InT>
struct Context {
    using ActualInT = typename replaceVoidWithPlaceholder<InT>::type;
    using ActualOutT = typename replaceVoidWithPlaceholder<OutT>::type;

    http::Method method;
    std::string uri;
    Params params;

    ActualInT body;
    Response<ActualOutT> res;
};

// ---------------------------------------------------------------------------------------------------------------------

}
