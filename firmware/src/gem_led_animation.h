#pragma once

#include "fix16.h"
#include <stdbool.h>

/* Routines for animating the LEDs on Gemini's front panel. */

enum gem_led_animation_mode {
    GEM_LED_MODE_NORMAL,
    GEM_LED_MODE_CALIBRATION,
    GEM_LED_MODE_HARD_SYNC,
    GEM_LED_MODE_TWEAK
};

extern struct gem_led_tweak_data_struct {
    fix16_t lfo_value;
    bool castor_pwm;
    bool pollux_pwm;
} gem_led_tweak_data;

void gem_led_animation_init();

void gem_led_animation_step();

void gem_led_animation_set_mode(enum gem_led_animation_mode mode);