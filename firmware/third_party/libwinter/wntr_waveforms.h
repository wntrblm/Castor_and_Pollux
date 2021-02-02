/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for generating waveforms.

    Each function takes a `phase` argument that is 0.0 -> 1.0 and indicates the
    position of the waveform. That means for sine you don't need to specify
    a value from 0.0 to 2 * PI.

    The "normalized" version of each function ensures that the output is always
    from 0.0 -> 1.0.
*/

#include "fix16.h"
#include "math.h"

/*
    Sine waveform from -1.0 to 1.0
*/
static inline fix16_t wntr_sine(fix16_t phase) { return fix16_sin(fix16_mul(phase, F16(M_PI * 2.0))); }

/*
    Normalized sine waveform from 0.0 to 1.0
*/
static inline fix16_t wntr_sine_normalized(fix16_t phase) {
    return fix16_div(fix16_add(wntr_sine(phase), F16(1.0)), F16(2.0));
}

/*
    Triangle waveform from -1.0 to 1.0
*/
static inline fix16_t wntr_triangle(fix16_t phase) {
    /* -1.0f + fabsf(-2.0f + fmodf(phase, 1.0f) * 4.0f); */
    return fix16_add(F16(-1.0), fix16_abs(fix16_add(F16(-2.0), fix16_mul(F16(4.0), fix16_mod(phase, F16(1.0))))));
}

/*
    Normalized triangle waveform from 0.0 to 1.0
*/
static inline fix16_t wntr_triangle_normalized(fix16_t phase) {
    return fix16_div(fix16_add(wntr_triangle(phase), F16(1.0)), F16(2.0));
}
