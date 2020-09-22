#pragma once

#include "fix16.h"
#include "math.h"

float gem_sine(fix16_t phase) { return fix16_sin(fix16_mul(phase, F16(2.0f * 3.14159265f))); }

float gem_triangle(fix16_t phase) {
    /* -1.0f + fabsf(-2.0f + fmodf(phase, 1.0f) * 4.0f); */
    return fix16_add(F16(-1.0f), fix16_abs(fix16_add(F16(-2.0f), fix16_mul(F16(4.0f), fix16_mod(phase, F16(1.0f))))));
}
