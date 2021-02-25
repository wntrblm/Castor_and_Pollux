/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_bezier.h"
#include <stddef.h>

#define F16_POW2(x) (fix16_mul(x, x))
#define F16_POW3(x) (fix16_mul(x, F16_POW2(x)))
#define ONE_MINUS(x) (fix16_sub(F16(1), x))
#define _MIN(a, b) ((a > b ? b : a))

fix16_t wntr_bezier_cubic_1d(fix16_t p0, fix16_t p1, fix16_t p2, fix16_t p3, fix16_t t) {
    /*
        Math.pow(1 - t, 3) * p0 +
        Math.pow(1 - t, 2) * t * 3 * p1 +
        Math.pow(t, 2) * (1 - t) * 3 * p2 +
        Math.pow(t, 3) * p3
    */
    fix16_t r0 = fix16_mul(F16_POW3(ONE_MINUS(t)), p0);
    fix16_t r1 = fix16_mul(fix16_mul(fix16_mul(F16_POW2(ONE_MINUS(t)), t), F16(3)), p1);
    fix16_t r2 = fix16_mul(fix16_mul(fix16_mul(F16_POW2(t), ONE_MINUS(t)), F16(3)), p2);
    fix16_t r3 = fix16_mul(F16_POW3(t), p3);
    return fix16_add(r0, fix16_add(r1, fix16_add(r2, r3)));
}
