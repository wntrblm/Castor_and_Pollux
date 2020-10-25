#pragma once

/*
    Routines for using the SAM D21 TCC peripheral to generate pulse/
    square wave/pwm signals.

    This module is current very specific to Gemini, but could easily
    be extended using the same channel descriptor model as gem_adc.h
*/

#include <stdbool.h>
#include <stdint.h>

/* Pulse (PWM) output using TCC peripheral. */

void gem_pulseout_init();

void gem_pulseout_set_period(uint8_t channel, uint32_t period);

void gem_pulseout_hard_sync(bool state);