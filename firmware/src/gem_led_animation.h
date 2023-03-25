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

#define GEM_MAX_DOTSTAR_COUNT 8

struct GemLEDCfg {
    uint32_t hue_offsets[GEM_MAX_DOTSTAR_COUNT];
    size_t vertical_pos_index[GEM_MAX_DOTSTAR_COUNT];
    size_t pwm_a_led;
    size_t pwm_b_led;
    size_t fm_a_led;
    size_t fm_b_led;
};

extern struct GemLEDInputs {
    fix16_t lfo_amplitude;
    fix16_t lfo_gain;
    uint16_t lfo_mod_a;
    uint16_t lfo_mod_b;
    bool tweaking;
} gem_led_inputs;

void gem_led_animation_init(const struct GemLEDCfg cfg);

bool gem_led_animation_step(const struct GemDotstarCfg* dotstar) RAMFUNC;

void gem_led_animation_set_mode(enum GemMode mode);
