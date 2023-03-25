/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include "gem_adc_channels.h"
#include "gem_mode.h"
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
    enum GemMode mode;
    uint16_t pitch_cv_code;
    uint16_t pitch_knob_code;
    uint16_t tweak_pitch_knob_code;
    uint16_t pulse_cv_code;
    uint16_t pulse_knob_code;
    uint16_t tweak_pulse_knob_code;
    uint16_t lfo_knob_code;
    uint16_t tweak_lfo_knob_code;
    fix16_t reference_pitch;
    fix16_t lfo_amplitude;
};

enum GemOscillatorPitchBehavior {
    GEM_PITCH_COARSE,
    GEM_PITCH_MULTIPLY,
    GEM_PITCH_FOLLOW,
    GEM_PITCH_FINE,
};

struct GemOscillator {
    /* Unchanging configuration */
    uint8_t number;
    fix16_t pitch_cv_min;
    fix16_t pitch_cv_max;
    bool can_follow;

    /* Configuration from settings */
    bool quantization_enabled;
    fix16_t pitch_offset;
    fix16_t pitch_knob_min;
    fix16_t pitch_knob_max;
    bool zero_detection_enabled;
    uint16_t zero_detection_threshold;
    uint16_t pulse_width_bitmask;
    fix16_t lfo_pitch_factor;

    /* State */
    uint32_t pulseout_period;
    uint16_t ramp_cv;
    fix16_t pitch;
    uint16_t pulse_width;
    enum GemOscillatorPitchBehavior pitch_behavior;
};

void gem_oscillator_init(struct WntrErrorCorrection pitch_cv_adc_error_correction, fix16_t pitch_knob_nonlinearity);
void GemOscillator_init(struct GemOscillator* osc);
void GemOscillator_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs) RAMFUNC;
void GemOscillator_post_update(const struct GemPulseOutConfig* pulseout, struct GemOscillator* osc) RAMFUNC;
