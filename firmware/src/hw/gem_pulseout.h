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

#include "wntr_ramfunc.h"
#include <stdbool.h>
#include <stdint.h>

/* Pulse (PWM) output using TCC peripheral. */

void gem_pulseout_init();

void gem_pulseout_set_period(uint8_t channel, uint32_t period) RAMFUNC;

void gem_pulseout_hard_sync(bool state) RAMFUNC;
