#pragma once

namespace mfl {

namespace helpers {

// ---------------------------------------------------------------------------------------------------------------------

template<int pin>
constexpr gpio_num_t pinFromInt() {
    static_assert(pin >=  GPIO_NUM_0, "pin number can not be nagative");
    static_assert(pin < GPIO_NUM_MAX, "pin number is larger than available pins");
    return (gpio_num_t) pin;
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace helpers

} // namespace mfl
