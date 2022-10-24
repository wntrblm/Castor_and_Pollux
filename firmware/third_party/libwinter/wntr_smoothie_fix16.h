/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    fixed-point version of Smoothie.
*/

#include "fix16.h"
#include "math.h"

struct WntrSmoothie {
    /* Base low-pass cutoff frequency. Called "F(c(min))" in the paper,
        decreasing reduces jitter. */
    fix16_t cutoff;

    /* The amount of influence the rate-of-change has on the cutoff frequency.
        Called "Beta" in the paper, increasing reduces lag. If set to 0,
        smoothing acts as a one pole low pass filter. */
    fix16_t sensitivity;

    /* Internal state. */
    fix16_t dx_cutoff;
    fix16_t _x_prev;
    fix16_t _dx_prev;
};

inline static void WntrSmoothie_init(struct WntrSmoothie* smooth, fix16_t initial_value) {
    if (smooth->dx_cutoff == 0) {
        smooth->dx_cutoff = F16(1.0f);
    }
    smooth->_x_prev = initial_value;
    smooth->_dx_prev = F16(0.0f);
};

inline static fix16_t _WntrSmoothie_calculate_alpha(fix16_t cutoff, fix16_t dt) {
    fix16_t tau = fix16_mul(F16(1.0f / 2.0f * 3.1415926f), cutoff);
    fix16_t r = fix16_mul(tau, dt);
    return fix16_div(r, fix16_add(r, F16(1.0)));
}

inline static fix16_t _WntrSmoothie_lowpass(fix16_t alpha, fix16_t* x_prev, fix16_t x) {
    x = fix16_add(fix16_mul(alpha, x), fix16_mul(fix16_sub(F16(1.0f), alpha), *x_prev));
    (*x_prev) = x;
    return x;
}

inline static fix16_t WntrSmoothie_step(struct WntrSmoothie* smooth, fix16_t dt, fix16_t x) {
    fix16_t dx = fix16_div(fix16_sub(x, smooth->_dx_prev), dt);
    fix16_t rate_of_change =
        _WntrSmoothie_lowpass(_WntrSmoothie_calculate_alpha(smooth->dx_cutoff, dt), &smooth->_dx_prev, dx);
    fix16_t cutoff = fix16_add(smooth->cutoff, fix16_mul(smooth->sensitivity, fix16_abs(rate_of_change)));
    return _WntrSmoothie_lowpass(_WntrSmoothie_calculate_alpha(cutoff, dt), &smooth->_x_prev, x);
}
