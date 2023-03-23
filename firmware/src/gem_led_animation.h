/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include "gem_dotstar.h"
#include "gem_mode.h"
#include "wntr_ramfunc.h"
#include <stdbool.h>

/* Routines for animating the LEDs on Gemini's front panel. */

extern struct GemLEDInputs {
    fix16_t lfo_amplitude;
    fix16_t lfo_gain;
    bool tweaking;
} gem_led_inputs;

void gem_led_animation_init();

bool gem_led_animation_step(const struct GemDotstarCfg* dotstar) RAMFUNC;

void gem_led_animation_set_mode(enum GemMode mode);
