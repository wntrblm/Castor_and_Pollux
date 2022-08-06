/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for controlling Dotstar (APA102C) RGB LEDs. */

#include "gem_spi.h"
#include <stddef.h>
#include <stdint.h>

struct GemDotstarCfg {
    size_t count;
    const struct GemSPIConfig* spi;
};

void gem_dotstar_init(uint8_t brightness);
void gem_dotstar_set(size_t n, uint8_t r, uint8_t g, uint8_t b);
/* Same as gem_dotstar_set, but take a whole 24-bit integer as the color value. */
void gem_dotstar_set32(size_t n, uint32_t color);
void gem_dotstar_update(const struct GemDotstarCfg* dotstar);
