#pragma once

/*
    Routines for interacting with the SAM D21's IO multiplexer and for
    reading and writing IO pins.
*/

#include "sam.h"
#include <stdbool.h>
#include <stdint.h>

#define GEM_PMUX_A PORT_PMUX_PMUXE_A_Val
#define GEM_PMUX_B PORT_PMUX_PMUXE_B_Val
#define GEM_PMUX_C PORT_PMUX_PMUXE_C_Val
#define GEM_PMUX_D PORT_PMUX_PMUXE_D_Val
#define GEM_PMUX_E PORT_PMUX_PMUXE_E_Val
#define GEM_PMUX_F PORT_PMUX_PMUXE_F_Val
#define GEM_PMUX_G PORT_PMUX_PMUXE_G_Val
#define GEM_PMUX_H PORT_PMUX_PMUXE_H_Val

static inline void gem_gpio_set_as_output(uint8_t port, uint8_t pin) { PORT->Group[port].DIRSET.reg = (1 << pin); }

static inline void gem_gpio_set_as_input(uint8_t port, uint8_t pin, bool pullup) {
    PORT->Group[port].DIRCLR.reg = (1 << pin);
    PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_INEN;
    if (pullup) {
        PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PULLEN;
        PORT->Group[port].OUTSET.reg = (1 << pin);
    }
}

static inline void gem_gpio_set(uint8_t port, uint8_t pin, bool value) {
    if (value) {
        PORT->Group[port].OUTSET.reg = (1 << pin);
    } else {
        PORT->Group[port].OUTCLR.reg = (1 << pin);
    }
}

static inline bool gem_gpio_get(uint8_t port, uint8_t pin) { return PORT->Group[port].IN.reg & (1 << pin); }

static inline void gem_gpio_set_mux(uint8_t port, uint8_t pin, uint8_t mux) {
    PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PMUXEN;
    if (pin & 1) {
        PORT->Group[port].PMUX[pin >> 1].bit.PMUXO = mux;
    } else {
        PORT->Group[port].PMUX[pin >> 1].bit.PMUXE = mux;
    }
}