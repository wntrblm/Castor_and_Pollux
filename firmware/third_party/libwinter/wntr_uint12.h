/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Helper macros for working with 12-bit values- common when dealing with
    ADCs/DACs.

    The "_F" variants operate on fix16_t values that cover the 12-bit integer
    range.
*/

#define UINT12_INVERT(value) (4095 - value)
#define UINT12_INVERT_F(value) fix16_sub(F16(4095), value)
#define UINT12_NORMALIZE_F(value) (fix16_div(value, F16(4095.0)))
#define UINT12_NORMALIZE(value) (UINT12_NORMALIZE_F(fix16_from_int(value)))
#define UINT12_CLAMP(value) value = value > 4095 ? 4095 : value
