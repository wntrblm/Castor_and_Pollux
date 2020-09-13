#pragma once

#include <stdint.h>

/* Pulse (PWM) output using TCC peripheral. */

void gem_pulseout_init();

void gem_pulseout_set_period(uint8_t channel, uint32_t period);

void gem_pulseout_phase_offset(float offset);