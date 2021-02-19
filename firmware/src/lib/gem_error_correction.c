/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_error_correction.h"

fix16_t gem_apply_error_correction_fix16(const fix16_t value, const struct GemErrorCorrection errors) {
    return fix16_mul(fix16_sub(value, errors.offset), errors.gain);
}

uint16_t gem_apply_error_correction_uint16(const uint16_t value, const struct GemErrorCorrection errors) {
    int32_t result = fix16_to_int(gem_apply_error_correction_fix16(fix16_from_int(value), errors));
    if (result < 0)
        result = 0;
    if (result > UINT16_MAX)
        result = UINT16_MAX;
    return (uint16_t)(result);
}
