/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Helper macros for processing ADC readings. */

#define GEM_ADC_INVERT(value) (4095 - value)
#define GEM_ADC_NORMALIZE_F16(value) (fix16_div(value, F16(4095.0)))
#define GEM_ADC_NORMALIZE_CODE(value) (GEM_ADC_NORMALIZE_F16(fix16_from_int(value)))
#define GEM_UINT12_CLAMP(value) value = value > 4095 ? 4095 : value
