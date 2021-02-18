/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Provides a 1-dimensional bezier curve for non-linear interpolation. */

#include "fix16.h"
#include <stddef.h>

/*
    Calculates the result of a 1-dimensional bezier curve with 4 points.
*/
fix16_t wntr_bezier_cubic_1d(fix16_t p0, fix16_t p1, fix16_t p2, fix16_t p3, fix16_t t);

/* Fills in a look-up table for the 1D, 2-control point bezier curve. */
void wntr_bezier_cubic_1d_generate_lut(fix16_t p0, fix16_t p1, fix16_t p2, fix16_t p3, fix16_t* buf, size_t buf_len);

/* Looks up and interpolates a 1D bezier from a lookup table for the interval t. */
fix16_t wntr_bezier_1d_lut_lookup(fix16_t* lut, size_t lut_len, fix16_t t);
