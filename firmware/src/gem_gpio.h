#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sam.h"


static inline void gem_gpio_set_as_output(uint8_t port, uint8_t pin) {
    PORT->Group[port].DIRSET.reg = (1 << pin);
}

static inline void gem_gpio_set_as_input(uint8_t port, uint8_t pin, bool pullup) {
    PORT->Group[port].DIRCLR.reg = (1 << pin);
    PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_INEN;
    if(pullup) PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PULLEN ;
}

static inline void gem_gpio_set(uint8_t port, uint8_t pin, bool value) {
    if (value) {
        PORT->Group[port].OUTSET.reg = (1 << pin);
    } else {
        PORT->Group[port].OUTCLR.reg = (1 << pin);
    }
}

static inline bool gem_gpio_get(uint8_t port, uint8_t pin) {
    return PORT->Group[port].IN.reg & (1 << pin);
}