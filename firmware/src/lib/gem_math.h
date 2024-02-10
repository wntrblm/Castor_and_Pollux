/*
    Copyright (c) 2022 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include <stdint.h>

/* Helpers for math operations, especially with integer or fixed point math */

/* Determines how far between `a` and `b` the given `value` is

    The result is an unsigned 16 bit integer where `0x0` means that `value == a`
    and `0xFFFF` means that `value == b`.

    Because this uses integer arithematic as a shortcut, (b - a) must be < 1.0.
 */
inline static uint16_t __attribute__((always_inline)) gem_f16_norm_dist_u16(fix16_t a, fix16_t b, fix16_t v) {
    if (a == b || v == b) {
        return 0xFFFF;
    }
    if (v == a) {
        return 0x0;
    }

    uint32_t dividend = (uint32_t)(v - a);
    dividend <<= 16;
    uint32_t divisor = (uint32_t)(b - a);
    return (uint16_t)(dividend / divisor);
}

/* Interpolates two 32-bit unsigned numbers using an unsigned 16 bit percentage.

    `t` should be a 16-bit number representing the percent of distance between
    `a` and `b` to approximate, where `result == a` if `t == 0x0` and
    `result == b` if `t == 0xFFFF`.
*/
inline static uint32_t __attribute__((always_inline)) gem_u32_lerp_u16(uint32_t a, uint32_t b, uint16_t t) {
    const uint32_t interval = b - a;
    const uint32_t t_u32 = (uint32_t)(t);
    return a + ((t_u32 * interval) >> 16);
}

inline static __attribute__((always_inline)) fix16_t gem_voct_to_frequency(fix16_t cv) {
    /*
        The basic formula for a given note's frequency is:

            f_n = f_0 * a^n

        Where:
        - f_n = the note frequency
        - f_0 = the reference note frequency
        - a = the scale division, 2^(1/12)
        - n = the number of semitones above or below the reference note

        With A440 as the reference note, this gives:

            f_n = 440 Hz * (2^(1/12))^n

        Since this formula requires semitones, the pitch control voltage needs
        to be converted to semitones relative to the reference note. Control
        voltage is 1 V per octave (12 semitones). Assuming C0 (MIDI note 12)
        is 0 V, the number of semitones above C1 is:

            semitones = 12 * cv

        The semitones need to be adjusted to be relative to the reference note
        of A4 (MIDI note 69) which corresponds to 4.75 V:

            semitones = 12 * (cv - 4.75)

        Plugging that into the formula gives:

            f_n = 440 Hz * (2^(1/12))^(12 * (cv - 4.75))

        Since the hardware this code runs on is quite limited, it's useful
        to do some optimization to avoid unnecessary operations.

        An easy win is to precompute the 2^(1/12) term:

            f_n = 440 Hz * 1.059463094^(12 * (cv - 4.75))

        The subtraction can be elimiated by swapping the reference note for C0
        (16.35159783 Hz):

            f_n = 16.35159783 Hz * 1.059463094^(12 * cv)

        Exponentiation is slow, but it can be approximated with natural log:

            f_n = 16.35159783 Hz Hz * e^(12 * cv * ln(1.059463094))

        This allows for a little more precomputation within the exponent, giving
        the final formula used here:

            f_n = 16.35159783 Hz * e^(cv * 0.6931471765)
    */

    return fix16_mul(F16(16.35159783), fix16_exp(fix16_mul(cv, F16(0.6931471765))));
}

inline static uint64_t __attribute__((always_inline)) gem_frequency_to_millihertz_f16_u64(fix16_t freq_hz) {
    uint32_t freq_whole = freq_hz >> 16;
    uint32_t freq_frac = freq_hz & 0xFFFF;
    return (freq_whole * 100) + (freq_frac * 100 / 0xFFFF);
}
