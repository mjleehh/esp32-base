#pragma once

#include <mfl/httpd/Context.hpp>
#include <mfl/helpers/macros.hpp>

#include <functional>

// ---------------------------------------------------------------------------------------------------------------------

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

template<typename InT, typename OutT = InT>
using Handler = std::function<void(Context<InT, OutT>& ctx)>;

// ---------------------------------------------------------------------------------------------------------------------

}
