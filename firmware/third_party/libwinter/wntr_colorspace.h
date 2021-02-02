/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for converting between different colorspaces. */

#include <stdint.h>

/*
    HSV to RGB.

    The hue is represented as a 16-bit number instead of the usual degrees.
    0 corresponds to 0 degrees and 65536 corresponds to 360 degrees.
*/
uint32_t wntr_colorspace_hsv_to_rgb(uint16_t hue, uint8_t sat, uint8_t val);
