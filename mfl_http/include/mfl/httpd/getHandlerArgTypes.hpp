#pragma once

#include <mfl/httpd/Context.hpp>
#include <type_traits>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

template<class A, class B>
struct indirect_false {
    static const bool value = false;
};

template<class A>
struct invalid_handler_function {
    using type = std::nullptr_t;
    static_assert(indirect_false<A, A>::value, "passed an invalid handler function");
};

// =====================================================================================================================

template <typename F>
struct getFunctorTypeArgs : getFunctorTypeArgs<decltype(&F::operator())> {};

template <typename FunctorT, typename RequestBodyT, typename ResponseBodyT>
struct getFunctorTypeArgs<void(FunctorT::*)(Context<RequestBodyT, ResponseBodyT>&) const> {
    using request_body_type  = RequestBodyT;
    using response_body_type = ResponseBodyT;
};

// =====================================================================================================================

template <typename FunctorT, typename RequestBodyT, typename ResponseBodyT>
struct getFunctorTypeArgs<void(FunctorT::*)(Context<RequestBodyT, ResponseBodyT>&)> {
    using request_body_type  = RequestBodyT;
    using response_body_type = ResponseBodyT;
};

// =====================================================================================================================

template<class F, bool isFunction>
struct getHandlerFunctionTypeArgs {
    using request_body_type = typename invalid_handler_function<F>::type;
};

template<class RequestBodyT, class ResponseBodyT>
struct getHandlerFunctionTypeArgs<void(*)(Context<RequestBodyT,ResponseBodyT>&), true> {
    using request_body_type  = RequestBodyT;
    using response_body_type = ResponseBodyT;
};

template<class F>
struct getHandlerFunctionTypeArgs<F, false> : getFunctorTypeArgs<F> {};

// =====================================================================================================================

/**
 * determine arguments and return type of a callable
 */
template<class F>
struct getHandlerArgTypes
        : getHandlerFunctionTypeArgs<F, std::is_function<typename std::remove_pointer<F>::type>::value> {};

// ---------------------------------------------------------------------------------------------------------------------

}
