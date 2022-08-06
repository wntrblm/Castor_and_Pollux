/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for interacting with SPI devices. */

#include "sam.h"
#include "wntr_gpio.h"
#include <stddef.h>
#include <stdint.h>

struct GemSPIConfig {
    uint32_t gclk;
    uint32_t gclk_freq;
    uint32_t baud;
    uint32_t polarity;
    uint32_t phase;
    Sercom* sercom;
    uint32_t apbcmask;
    uint32_t clkctrl_id;
    uint32_t dopo;
    struct WntrGPIOPin sck_pin;
    struct WntrGPIOPin sdo_pin;
};

void gem_spi_init(const struct GemSPIConfig* spi);

void gem_spi_write(const struct GemSPIConfig* spi, const uint8_t* data, size_t len);
