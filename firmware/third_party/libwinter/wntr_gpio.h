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

#ifdef SAMD21
#include "wntr_gpio_samd21.h"
#endif

void wntr_gpio_set_as_output(uint8_t port, uint8_t pin);

void wntr_gpio_set_as_input(uint8_t port, uint8_t pin, bool pullup);

void wntr_gpio_set(uint8_t port, uint8_t pin, bool value);

bool wntr_gpio_get(uint8_t port, uint8_t pin);

void wntr_gpio_configure_alt(uint8_t port, uint8_t pin, uint8_t alt);
