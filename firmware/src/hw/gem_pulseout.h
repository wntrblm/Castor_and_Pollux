/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for using the SAM D21 TCC peripheral to generate pulse/
    square wave/pwm signals.

    This module is current very specific to Gemini, but could easily
    be extended using the same channel descriptor model as gem_adc.h
*/

#include "gem_config.h"
#include "wntr_ramfunc.h"
#include <stdbool.h>
#include <stdint.h>

/* Pulse (PWM) output using TCC peripheral. */

typedef void (*gem_pulseout_ovf_callback)(uint8_t inst);

void gem_pulseout_init(gem_pulseout_ovf_callback ovf_callback);
void gem_pulseout_set_period(uint8_t channel, uint32_t period) RAMFUNC;

inline static uint32_t gem_pulseout_frequency_to_period(uint32_t freq_millihertz) {
    return (((GEM_TCC_GCLK_FREQUENCY * 100) / freq_millihertz) - 1);
}

inline static void gem_pulseout_set_frequency(uint8_t channel, uint32_t freq_millihertz) {
    gem_pulseout_set_period(channel, gem_pulseout_frequency_to_period(freq_millihertz));
}
