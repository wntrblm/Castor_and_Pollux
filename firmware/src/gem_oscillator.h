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
    fix16_t cv_min;
    fix16_t cv_max;
};

struct GemOscillatorInputs {
    uint32_t* adc;
    fix16_t lfo_pitch;
    fix16_t lfo_pulse_width;
};

struct GemOscillator {
    /* Unchanging configuration */
    uint8_t number;
    enum GemADCChannel pitch_cv_channel;
    enum GemADCChannel pitch_knob_channel;
    enum GemADCChannel pulse_width_cv_channel;
    enum GemADCChannel pulse_width_knob_channel;
    fix16_t cv_min;
    fix16_t cv_max;

    /* Configuration from settings */
    fix16_t cv_base_offset;
    fix16_t knob_min;
    fix16_t knob_range;
    uint16_t follower_threshold;
    bool lfo_pitch;
    bool lfo_pwm;
    uint16_t pulse_width_bitmask;

    /* State */
    uint32_t pulseout_period;
    uint16_t ramp_cv;
    fix16_t pitch_cv;
    fix16_t pitch_knob;
    fix16_t pitch;
    uint16_t pulse_width_knob;
    uint16_t pulse_width_cv;
    uint16_t pulse_width;
};

void gem_oscillator_init(struct WntrErrorCorrection pitch_cv_adc_error_correction, fix16_t pitch_knob_nonlinearity);

void GemOscillator_init(
    struct GemOscillator* osc,
    uint8_t number,
    enum GemADCChannel pitch_cv_channel,
    enum GemADCChannel pitch_knob_channel,
    enum GemADCChannel pulse_width_cv_channel,
    enum GemADCChannel pulse_width_knob_channel,
    fix16_t cv_base_offset,
    fix16_t cv_min,
    fix16_t cv_max,
    fix16_t knob_min,
    fix16_t knob_max,
    bool lfo_pwm,
    uint16_t pulse_width_bitmask);

void GemOscillator_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs) RAMFUNC;

void GemOscillator_post_update(
    const struct GemPulseOutConfig* pulseout, struct GemOscillator* osc, struct GemOscillatorInputs inputs) RAMFUNC;
