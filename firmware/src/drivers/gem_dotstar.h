#pragma once

/* Routines for controlling Dotstar (APA102C) RGB LEDs. */

#include <stddef.h>
#include <stdint.h>

void gem_dotstar_init(uint8_t brightness);
void gem_dotstar_set(size_t n, uint8_t r, uint8_t g, uint8_t b);
/* Same as gem_dotstar_set, but take a whole 24-bit integer as the color value. */
void gem_dotstar_set32(size_t n, uint32_t color);
void gem_dotstar_update();