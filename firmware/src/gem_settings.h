#pragma once

/* Routines for managing NVM-backed user-changeable settings. */

#include "fix16.h"
#include <stdbool.h>
#include <stdint.h>

/* When adding/modifying fields be sure to update:
    _default_settings
    gem_settings_*()
    libgemini/gemini.py
*/
struct gem_settings {
    /* Configure for the ADC's internal error correction registers. */
    uint16_t adc_gain_corr;
    uint16_t adc_offset_corr; /* code points. */
    /* The front-plate LED brightness. */
    uint16_t led_brightness; /* 0-255 */
    /* Configuration for the CV knob mins and maxs, defaults to -1.01 to +1.01. */
    fix16_t castor_knob_min; /* volts */
    fix16_t castor_knob_max; /* volts */
    fix16_t pollux_knob_min; /* volts */
    fix16_t pollux_knob_max; /* volts */
    /* Chorusing configuration. */
    fix16_t chorus_max_intensity; /* volts */
    fix16_t chorus_frequency;     /* hertz */
    /* Error correction for the ADC readings for the CV and PWM knobs. */
    fix16_t knob_offset_corr; /* volts */
    fix16_t knob_gain_corr;
    /* Pitch input CV smoothing parameters. */
    fix16_t smooth_initial_gain;
    fix16_t smooth_sensitivity;
    /* This is the "deadzone" for Pollux's pitch CV input, basically, it
       should be around 0v and it's the point where Pollux starts following
       Castor's pitch CV. By default this is 6 code points to allow some
       variance in time and temperature. */
    uint16_t pollux_follower_threshold; /* code points */
    /* Route LFO to PWM for oscillators */
    bool castor_lfo_pwm;
    bool pollux_lfo_pwm;
};

bool gem_settings_deserialize(struct gem_settings* settings, uint8_t* data);
void gem_settings_serialize(struct gem_settings* settings, uint8_t* data);
bool gem_settings_load(struct gem_settings* settings);
void gem_settings_save(struct gem_settings* settings);
void gem_settings_erase();
void gem_settings_print(struct gem_settings* settings);