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
