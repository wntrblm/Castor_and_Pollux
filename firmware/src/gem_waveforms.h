#pragma once

#include "fix16.h"
#include "math.h"

static inline fix16_t gem_sine(fix16_t phase) { return fix16_sin(fix16_mul(phase, F16(M_PI * 2.0))); }

static inline fix16_t gem_sine_norm(fix16_t phase) { return fix16_div(fix16_add(gem_sine(phase), F16(1.0)), F16(2.0)); }

static inline fix16_t gem_triangle(fix16_t phase) {
    /* -1.0f + fabsf(-2.0f + fmodf(phase, 1.0f) * 4.0f); */
    return fix16_add(F16(-1.0f), fix16_abs(fix16_add(F16(-2.0f), fix16_mul(F16(4.0f), fix16_mod(phase, F16(1.0f))))));
}
