#pragma once

/* Implementation of "Dynamic Smoothing Using Self Modulated Filter"
    https://cytomic.com/files/dsp/DynamicSmoothing.pdf
*/

#include "fix16.h"

struct GemSmoothie {
    /* 0.0 - 1.0, with 0.0 preventing any changes and 1.0 bypassing the filter. */
    fix16_t initial_gain;
    /* > 0.0, this value is highly dependent on the scale of the input. */
    fix16_t sensitivity;
    /* Internal state. */
    fix16_t _lowpass1;
    fix16_t _lowpass2;
};

inline static fix16_t GemSmoothie_step(struct GemSmoothie* state, fix16_t val) {
    fix16_t band = fix16_abs(fix16_sub(state->_lowpass1, state->_lowpass2));
    fix16_t g = fix16_min(fix16_add(state->initial_gain, fix16_mul(state->sensitivity, band)), F16(1.0));
    state->_lowpass1 = fix16_add(fix16_mul(g, val), fix16_mul(fix16_sub(F16(1.0), g), state->_lowpass1));
    state->_lowpass2 = fix16_add(fix16_mul(g, state->_lowpass1), fix16_mul(fix16_sub(F16(1.0), g), state->_lowpass2));
    return state->_lowpass2;
}
