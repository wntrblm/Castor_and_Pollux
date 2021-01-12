#include "gem_bezier.h"
#include <stddef.h>

#define F16_POW2(x) (fix16_mul(x, x))
#define F16_POW3(x) (fix16_mul(x, F16_POW2(x)))
#define ONE_MINUS(x) (fix16_sub(F16(1), x))
#define _MIN(a, b) ((a > b ? b : a))

fix16_t gem_bezier_1d_2c(fix16_t c1, fix16_t c2, fix16_t t) {
    /* return 3 * c1 * t * Math.pow(1 - t, 2)   +   3 * c2 * (1 - t) * Math.pow(t, 2)  +   Math.pow(t, 3); */
    fix16_t p0 = fix16_mul(F16(3), fix16_mul(c1, fix16_mul(t, F16_POW2(ONE_MINUS(t)))));
    fix16_t p1 = fix16_mul(F16(3), fix16_mul(c2, fix16_mul(ONE_MINUS(t), F16_POW2(t))));
    fix16_t p2 = F16_POW3(t);
    return fix16_add(fix16_add(p0, p1), p2);
}

void gem_bezier_1d_2c_generate_lut(fix16_t c1, fix16_t c2, fix16_t* buf, size_t buf_len) {
    fix16_t buf_len_f = fix16_from_int((int)(buf_len));
    for (size_t i = 0; i < buf_len; i++) {
        fix16_t f = fix16_div(fix16_from_int((int)(i)), buf_len_f);
        buf[i] = gem_bezier_1d_2c(c1, c2, f);
    }
};

fix16_t gem_bezier_1d_lut_lookup(fix16_t* lut, size_t lut_len, fix16_t t) {
    t = fix16_clamp(t, F16(0), F16(1));
    fix16_t lut_len_f = fix16_from_int((int)(lut_len));
    fix16_t t_mul_len = fix16_mul(lut_len_f, t);
    fix16_t floored = fix16_floor(t_mul_len);
    fix16_t frac_f = fix16_sub(t_mul_len, floored);
    uint8_t frac = ((uint32_t)(frac_f)&0xFFFF) >> 8;
    size_t low_idx = fix16_to_int(floored);
    size_t high_idx = fix16_to_int(fix16_ceil(t_mul_len));
    fix16_t low = lut[_MIN(low_idx, lut_len - 1)];
    fix16_t high = lut[_MIN(high_idx, lut_len - 1)];
    if (low_idx == lut_len) {
        return F16(1.0);
    }
    if (low_idx == high_idx || frac <= 1) {
        return low;
    }
    if (frac >= 254) {
        return high;
    }
    return fix16_lerp8(low, high, frac);
}
