/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for converting between different colorspaces. */

#include <stdint.h>

uint32_t gem_colorspace_hsv_to_rgb(uint16_t hue, uint8_t sat, uint8_t val);
