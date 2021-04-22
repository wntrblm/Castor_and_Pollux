/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Generic functions for interacting with a device's GPIO.
*/

#include <stdbool.h>
#include <stdint.h>

#if defined(SAMD21) || defined(SAMD51)
#include "wntr_gpio_samd.h"
#endif

struct WntrGPIOPin {
    uint8_t port;
    uint8_t pin;
};

#define WNTR_GPIO_PIN(port_, pin_) ((struct WntrGPIOPin){.port = port_, .pin = pin_})

void wntr_gpio_set_as_output(uint8_t port, uint8_t pin);
void wntr_gpio_set_as_input(uint8_t port, uint8_t pin, bool pullup);
void wntr_gpio_set(uint8_t port, uint8_t pin, bool value);
bool wntr_gpio_get(uint8_t port, uint8_t pin);
void wntr_gpio_configure_alt(uint8_t port, uint8_t pin, uint8_t alt);

/* Aliases for the above functions that use WntrGPIOPin */
inline static void WntrGPIOPin_set_as_output(const struct WntrGPIOPin pin) {
    wntr_gpio_set_as_output(pin.port, pin.pin);
}
inline static void WntrGPIOPin_set_as_input(const struct WntrGPIOPin pin, bool pullup) {
    wntr_gpio_set_as_input(pin.port, pin.pin, pullup);
}
inline static void WntrGPIOPin_set(const struct WntrGPIOPin pin, bool value) {
    wntr_gpio_set(pin.port, pin.pin, value);
}
inline static bool WntrGPIOPin_get(const struct WntrGPIOPin pin) { return wntr_gpio_get(pin.port, pin.pin); }
inline static void WntrGPIOPin_configure_alt(const struct WntrGPIOPin pin, uint8_t alt) {
    return wntr_gpio_configure_alt(pin.port, pin.pin, alt);
}
