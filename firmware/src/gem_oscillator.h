/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include "gem_adc_channels.h"
#include "gem_pulseout.h"
#include "wntr_error_correction.h"
#include "wntr_ramfunc.h"
#include <stdbool.h>
#include <stdint.h>

/* The core logic for updating Gemini's oscillators based on external input. */

struct GemOscillatorInputConfig {
    fix16_t pitch_cv_min;
    fix16_t pitch_cv_max;
};

struct GemOscillatorInputs {
    uint16_t pitch_cv_code;
    uint16_t pitch_knob_code;
    uint16_t pulse_cv_code;
    uint16_t pulse_knob_code;
    fix16_t reference_pitch;
    fix16_t lfo_phase;
    fix16_t lfo_amplitude;
};

struct GemOscillator {
    /* Unchanging configuration */
    uint8_t number;
    fix16_t pitch_cv_min;
    fix16_t pitch_cv_max;
    bool can_follow;

    /* Configuration from settings */
    fix16_t pitch_offset;
    fix16_t pitch_knob_min;
    fix16_t pitch_knob_max;
    uint16_t nonzero_threshold;
    uint16_t pulse_width_bitmask;
    fix16_t lfo_pitch_factor;
    bool apply_lfo_to_pitch;
    bool apply_lfo_to_pulse;

    /* State */
    uint32_t pulseout_period;
    uint16_t ramp_cv;
    fix16_t pitch;
    uint16_t pulse_width;
};

void gem_oscillator_init(struct WntrErrorCorrection pitch_cv_adc_error_correction, fix16_t pitch_knob_nonlinearity);

void GemOscillator_init(struct GemOscillator* osc);

fix16_t gem_oscillator_calc_pitch_cv(fix16_t cv_min, fix16_t cv_max, uint16_t adc_code) RAMFUNC;
fix16_t
gem_oscillator_calc_pitch_knob(fix16_t knob_min, fix16_t knob_range, fix16_t nonlinearity, uint16_t adc_code) RAMFUNC;
fix16_t gem_oscillator_calc_pitch(fix16_t base_offset, fix16_t pitch_cv, fix16_t pitch_knob) RAMFUNC;
uint16_t gem_oscillator_calculate_pulse_width(
    uint16_t cv_adc_code, uint16_t knob_adc_code, bool apply_lfo, fix16_t lfo, uint16_t bitmask) RAMFUNC;

void GemOscillator_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs) RAMFUNC;

void GemOscillator_post_update(const struct GemPulseOutConfig* pulseout, struct GemOscillator* osc) RAMFUNC;
