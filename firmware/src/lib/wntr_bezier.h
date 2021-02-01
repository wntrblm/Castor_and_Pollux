/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Provides a 1-dimensional bezier curve for non-linear interpolation. */

#include "fix16.h"
#include <stddef.h>

/* Calculates the result of a 1-dimensional bezier curve with 2 control points at the given interval t. */
fix16_t wntr_bezier_1d_2c(fix16_t c1, fix16_t c2, fix16_t t);

/* Fills in a look-up table for the 1D, 2-control point bezier curve. */
void wntr_bezier_1d_2c_generate_lut(fix16_t c1, fix16_t c2, fix16_t* buf, size_t buf_len);

/* Looks up and interpolates a 1D bezier from a lookup table for the interval t. */
fix16_t wntr_bezier_1d_lut_lookup(fix16_t* lut, size_t lut_len, fix16_t t);
