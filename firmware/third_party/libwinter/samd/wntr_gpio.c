/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/
/*
    Implementation of wntr_gpio for the SAM D21/D51.
*/

#include "sam.h"

#include "wntr_gpio.h"

void wntr_gpio_set_as_output(uint8_t port, uint8_t pin) { PORT->Group[port].DIRSET.reg = (1 << pin); }

void wntr_gpio_set_as_input(uint8_t port, uint8_t pin, bool pullup) {
    PORT->Group[port].DIRCLR.reg = (1 << pin);
    PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_INEN;
    if (pullup) {
        PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PULLEN;
        PORT->Group[port].OUTSET.reg = (1 << pin);
    }
}

void wntr_gpio_set(uint8_t port, uint8_t pin, bool value) {
    if (value) {
        PORT->Group[port].OUTSET.reg = (1 << pin);
    } else {
        PORT->Group[port].OUTCLR.reg = (1 << pin);
    }
}

bool wntr_gpio_get(uint8_t port, uint8_t pin) { return PORT->Group[port].IN.reg & (1 << pin); }

void wntr_gpio_configure_alt(uint8_t port, uint8_t pin, uint8_t alt) {
    PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PMUXEN;
    if (pin & 1) {
        PORT->Group[port].PMUX[pin >> 1].bit.PMUXO = alt;
    } else {
        PORT->Group[port].PMUX[pin >> 1].bit.PMUXE = alt;
    }
}
