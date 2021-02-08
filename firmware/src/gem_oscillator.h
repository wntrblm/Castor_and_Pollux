/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include "gem_config.h"
#include "wntr_smoothie.h"
#include <stdbool.h>
#include <stdint.h>

/* The core logic for updating Gemini's oscillators based on external input. */

struct GemOscillatorInputs {
    uint32_t* adc;
    fix16_t lfo_pitch;
    fix16_t lfo_pulse_width;
};

struct GemOscillatorOutputs {
    fix16_t pitch_cv;
    uint32_t period;
    uint16_t ramp_cv;
};

struct GemOscillator {
    /* Configuration */
    uint8_t number;
    enum GemADCChannels pitch_cv_channel;
    enum GemADCChannels pitch_knob_channel;
    enum GemADCChannels pulse_width_cv_channel;
    enum GemADCChannels pulse_width_knob_channel;
    fix16_t knob_min;
    fix16_t knob_range;
    uint16_t follower_threshold;
    bool lfo_pitch;
    bool lfo_pwm;

    /* State */
    struct GemOscillatorOutputs outputs;
    struct WntrSmoothie smooth;
    fix16_t pitch_knob;
    fix16_t pitch_cv;
    fix16_t pitch;
    uint16_t pulse_width_knob;
    uint16_t pulse_width_cv;
    uint16_t pulse_width;
};

void gem_oscillator_init(struct GemADCErrors pitch_cv_adc_errors, fix16_t pitch_knob_nonlinearity);

void GemOscillator_init(
    struct GemOscillator* osc,
    uint8_t number,
    enum GemADCChannels pitch_cv_channel,
    enum GemADCChannels pitch_knob_channel,
    enum GemADCChannels pulse_width_cv_channel,
    enum GemADCChannels pulse_width_knob_channel,
    fix16_t smooth_initial_gain,
    fix16_t smooth_sensitivity,
    fix16_t knob_min,
    fix16_t knob_max,
    bool lfo_pwm);

void GemOscillator_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs);

void GemOscillator_post_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs);

void GemOscillatorOutputs_calculate(uint8_t osc, fix16_t pitch_cv, struct GemOscillatorOutputs* out);
