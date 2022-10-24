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

#include "math.h"

struct WntrSmoothie {
    /* Base low-pass cutoff frequency. Called "F(c(min))" in the paper,
        decreasing reduces jitter. */
    float cutoff;

    /* The amount of influence the rate-of-change has on the cutoff frequency.
        Called "Beta" in the paper, increasing reduces lag. If set to 0,
        smoothing acts as a one pole low pass filter. */
    float sensitivity;

    /* Internal state. */
    float dx_cutoff;
    float _x_prev;
    float _dx_prev;
};

inline static void WntrSmoothie_init(struct WntrSmoothie* smooth, float initial_value) {
    if (smooth->dx_cutoff == 0) {
        smooth->dx_cutoff = 1.0f;
    }
    smooth->_x_prev = initial_value;
    smooth->_dx_prev = 0.0f;
};

inline static float _WntrSmoothie_calculate_alpha(float cutoff, float dt) {
    float tau = 1.0f / 2.0f * 3.1415926f * cutoff;
    float r = tau * dt;
    return r / (r + 1);
}

inline static float _WntrSmoothie_lowpass(float alpha, float* x_prev, float x) {
    x = alpha * x + (1.0f - alpha) * (*x_prev);
    (*x_prev) = x;
    return x;
}

inline static float WntrSmoothie_step(struct WntrSmoothie* smooth, float dt, float x) {
    float dx = (x - smooth->_x_prev) / dt;
    float rate_of_change =
        _WntrSmoothie_lowpass(_WntrSmoothie_calculate_alpha(smooth->dx_cutoff, dt), &smooth->_dx_prev, dx);
    float cutoff = smooth->cutoff + smooth->sensitivity * fabs(rate_of_change);
    return _WntrSmoothie_lowpass(_WntrSmoothie_calculate_alpha(cutoff, dt), &smooth->_x_prev, x);
}
