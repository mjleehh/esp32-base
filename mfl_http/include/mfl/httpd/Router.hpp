#pragma once

#include <mfl/httpd/HttpLayer.hpp>
#include <mfl/httpd/getHandlerArgTypes.hpp>
#include <mfl/httpd/PathNode.hpp>
#include <mfl/http/Method.hpp>
#include <mfl/http/response_types/response-types.hpp>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Router middleware for HTTP server.
 *
 * Allows gin like requests.
 */
struct Router {
    Router();

    template<class F>
    void post(const std::string& uriTemplate, F handler) {
        addHandler(http::Method::post, uriTemplate, handler);
    }

    template<class F>
    void get(const std::string& uriTemplate, F handler) {
        addHandler(http::Method::get, uriTemplate, handler);
    }

    template<class F>
    void put(const std::string& uriTemplate, F handler) {
        addHandler(http::Method::put, uriTemplate, handler);
    }

    template<class F>
    void del(const std::string& uriTemplate, F handler) {
        addHandler(http::Method::del, uriTemplate, handler);
    }

    void handle(WrapperContext& context) const;
private:
    template<class F>
    void addHandler(http::Method method, const std::string& uriTemplate, F handler) {
        addHandlerWrapper(method, uriTemplate, [handler](WrapperContext& wrapperContext) {
            using RequestBodyT = typename getHandlerArgTypes<F>::request_body_type;
            using ResponseBodyT = typename getHandlerArgTypes<F>::response_body_type;
            using ActualInT = typename Context<RequestBodyT, ResponseBodyT>::ActualInT;
            using ActualOutT = typename Context<RequestBodyT, ResponseBodyT>::ActualOutT;

            Context<RequestBodyT, ResponseBodyT> context(std::move(wrapperContext.params), http::response_types::deserialize<ActualInT>(wrapperContext.body));
            handler(context);
            wrapperContext.res.returnType = http::response_types::getMimeType<ActualOutT>();
            wrapperContext.res.body = http::response_types::serialize<const ActualOutT>(context.res.body);
        });
    }

    void addHandlerWrapper(http::Method method, const std::string& uriTemplate, const HandlerWrapper& handlerWrapper);

    PathNode root_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
