/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Smoothie is a fancy smoothing filter.

    It's great for smoothing input that should react quickly to large changes-
    for example, inputs from knobs or faders, while maintaining typical
    low-pass noise reduction.

    This is an implementation of "Dynamic Smoothing Using Self Modulated Filter"
    Reference: https://cytomic.com/files/dsp/DynamicSmoothing.pdf
*/

#include "fix16.h"

struct WntrSmoothie {
    /*
        The baseline gain applied to new values by the filter.

        Should be between 0.0 and 1.0, with 0.0 preventing any changes and 1.0
        bypassing the filter.
    */
    fix16_t initial_gain;
    /*
        The sensitivity of the filter.

        This is used to determine the amount of modulation that is applied to
        the baseline gain. This is dependent on the scale of the inputs -
        for example, if the inputs can be between 0 and 10 then the sensitivity
        should be somewhere within that range, with 0 being unreactive to new
        inputs and 10 being very "twitchy".

    */
    fix16_t sensitivity;

    /* Internal state of the lowpass filters. */
    fix16_t _lowpass1;
    fix16_t _lowpass2;
};

inline static fix16_t WntrSmoothie_step(struct WntrSmoothie* state, fix16_t val) {
    fix16_t band = fix16_abs(fix16_sub(state->_lowpass1, state->_lowpass2));
    fix16_t g = fix16_min(fix16_add(state->initial_gain, fix16_mul(state->sensitivity, band)), F16(1.0));
    state->_lowpass1 = fix16_add(fix16_mul(g, val), fix16_mul(fix16_sub(F16(1.0), g), state->_lowpass1));
    state->_lowpass2 = fix16_add(fix16_mul(g, state->_lowpass1), fix16_mul(fix16_sub(F16(1.0), g), state->_lowpass2));
    return state->_lowpass2;
}
