#pragma once

#include <mfl/httpd/Response.hpp>
#include <mfl/http/Method.hpp>
#include <mfl/httpd/Params.hpp>

namespace mfl::httpd {

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

    Context(Params&& params, ActualInT&& body): params(params), body(body) {}

    Params params;

    ActualInT body;
    Response<ActualOutT> res;
};

// ---------------------------------------------------------------------------------------------------------------------

struct WrapperContext {
    http::Method method;
    std::string uri;

    Params params;
    std::string body;
    WrapperResponse res;
};

// ---------------------------------------------------------------------------------------------------------------------

}
