/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for applying basic linear error correction to ADCs/DACs. */

#include "fix16.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct GemErrorCorrection {
    /* Offset correction in code points */
    fix16_t offset;
    /* Gain correction as a multiplication factor, generally between 0.5 and 1.5 */
    fix16_t gain;
};

fix16_t gem_apply_error_correction_fix16(const fix16_t value, const struct GemErrorCorrection errors);
uint16_t gem_apply_error_correction_uint16(const uint16_t value, const struct GemErrorCorrection errors);
