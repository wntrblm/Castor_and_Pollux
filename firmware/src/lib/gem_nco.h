/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "stdint.h"

/* Simple phase-accumulation NCO */

struct GemNCO {
    uint32_t phase;
    uint32_t phase_step;
};

/* Advances the NCO by one phase step.

    Returns true if the phase wrapped around.
*/
inline static __attribute__((always_inline)) bool GemNCO_step(struct GemNCO* nco) {
    nco->phase += nco->phase_step;
    return nco->phase < nco->phase_step;
}

/* Calculates the phase step given the target frequency (in milliHz) and clock frequency (in milliHz)

    The basic tuning equation for an NCO is:

        output_freq = (phase_step * clock_freq) / 2^32

    Re-arranged to solve for phase step:

        phase_step = output_freq * 2^32 / clock_freq

    milliHz is used to maintain frequency resolution while doing integer-only math.
*/
inline static void GemNCO_set_frequency(struct GemNCO* nco, uint64_t target_freq, uint64_t clock_freq) {
    nco->phase_step = (uint32_t)(target_freq * (1ull << 32) / clock_freq);
}
