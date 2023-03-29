/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_oscillator.h"
#include "gem_config.h"
#include "gem_math.h"
#include "gem_pulseout.h"
#include "gem_ramp_table.h"
#include "wntr_bezier.h"
#include "wntr_uint12.h"
#include <printf.h>

/* Static variables */

static fix16_t pitch_knob_nonlinearity_;
static struct WntrErrorCorrection pitch_cv_adc_errors_;

/* Forward declarations */

static void GemOscillator_update_pitch_(struct GemOscillator* osc, const struct GemOscillatorInputs inputs) RAMFUNC;
static fix16_t gem_oscillator_calc_pitch_cv_(fix16_t cv_min, fix16_t cv_max, uint16_t adc_code) RAMFUNC;
static fix16_t
gem_oscillator_calc_pitch_knob_(fix16_t knob_min, fix16_t knob_range, fix16_t nonlinearity, uint16_t adc_code) RAMFUNC;
static void
GemOscillator_update_pulse_width_(struct GemOscillator* osc, const struct GemOscillatorInputs inputs) RAMFUNC;

/* Public functions */

void gem_oscillator_init(struct WntrErrorCorrection pitch_cv_adc_error_correction, fix16_t pitch_knob_nonlinearity) {
    pitch_cv_adc_errors_ = pitch_cv_adc_error_correction;
    pitch_knob_nonlinearity_ = pitch_knob_nonlinearity;
}

void GemOscillator_init(struct GemOscillator* osc) {
    osc->ramp_cv = 0;
    osc->pitch = F16(0);
    osc->pulse_width = 2048;
}

void GemOscillator_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs) {
    GemOscillator_update_pitch_(osc, inputs);
    GemOscillator_update_pulse_width_(osc, inputs);
}

void GemOscillator_post_update(const struct GemPulseOutConfig* pulseout, struct GemOscillator* osc) {
    // Use the note and charge look-up tables to calculate the outputs for the
    // oscillator.
    fix16_t freq_hz = gem_voct_to_frequency(osc->pitch);
    uint64_t freq_millihz = gem_frequency_to_millihertz_f16_u64(freq_hz);

    osc->pulseout_period = gem_pulseout_frequency_to_period(pulseout, freq_millihz);
    osc->ramp_cv = gem_ramp_table_lookup(osc->number, osc->pitch);
}

static void GemOscillator_update_pitch_(struct GemOscillator* osc, const struct GemOscillatorInputs inputs) {
    fix16_t pitch;
    fix16_t fm_amount = F16(0);
    fix16_t base_offset = osc->pitch_offset;
    bool is_castor = osc->number == 0;
    bool is_pollux = osc->number == 1;
    bool is_hard_sync = inputs.mode == GEM_MODE_HARD_SYNC;

    bool is_zero = osc->zero_detection_enabled && (UINT12_INVERT(inputs.pitch_cv_code) < osc->zero_detection_threshold);

    // "coarse" pitch behavior is used when the pitch jack isn't connected to
    // Castor
    if (is_castor && is_zero) {
        osc->pitch_behavior = GEM_PITCH_COARSE;

        pitch = gem_oscillator_calc_pitch_knob_(F16(0), F16(6), 0, inputs.pitch_knob_code);

        // quantize
        if (osc->quantization_enabled) {
            pitch = fix16_mul(pitch, F16(12));
            pitch = fix16_floor(fix16_add(pitch, F16(0.5)));
            pitch = fix16_div(pitch, F16(12));
        }
    }

    // "multiply" behavior is used by Pollux in hard sync mode. It's similar to
    // "follow" but doesn't allow Pollux's frequency to go below Castor's but
    // instead sets Pollux's frequency 0 to 3 octaves higher than Castor's.
    else if (is_pollux && is_hard_sync) {
        osc->pitch_behavior = GEM_PITCH_MULTIPLY;

        if (is_zero) {
            pitch = inputs.reference_pitch;
            // Importantly, this does *not* add the base pitch offset.
            base_offset = F16(0);
        } else {
            pitch = gem_oscillator_calc_pitch_cv_(osc->pitch_cv_min, osc->pitch_cv_max, inputs.pitch_cv_code);
        }

        pitch = fix16_add(pitch, gem_oscillator_calc_pitch_knob_(F16(0), F16(3), 0, inputs.pitch_knob_code));
    }

    // "follow" behavior is used by Pollux when both Castor & Pollux don't have
    // any pitch CV patched.
    else if (is_pollux && is_zero) {
        osc->pitch_behavior = GEM_PITCH_FOLLOW;

        pitch = fix16_add(
            inputs.reference_pitch,
            gem_oscillator_calc_pitch_knob_(
                osc->pitch_knob_min, osc->pitch_knob_max, pitch_knob_nonlinearity_, inputs.pitch_knob_code));
        // Importantly, this does *not* add the base pitch offset.
        base_offset = F16(0);
    }

    // "fine" pitch behavior is used when the pitch jack is connected to a CV
    // source.
    else {
        osc->pitch_behavior = GEM_PITCH_FINE;

        pitch = gem_oscillator_calc_pitch_cv_(osc->pitch_cv_min, osc->pitch_cv_max, inputs.pitch_cv_code);
        pitch = fix16_add(
            pitch,
            gem_oscillator_calc_pitch_knob_(
                osc->pitch_knob_min, osc->pitch_knob_max, pitch_knob_nonlinearity_, inputs.pitch_knob_code));
    }

    // In normal and hard sync modes, use the LFO to modulate only Pollux's
    // pitch with the intensity controlled by the LFO knob.
    if ((inputs.mode == GEM_MODE_NORMAL || inputs.mode == GEM_MODE_HARD_SYNC) && is_pollux) {
        fm_amount = UINT12_NORMALIZE(inputs.lfo_knob_code);
    }

    // In LFO FM mode, use the LFO to modulate pitch with the intensity
    // controlled by the pulse width knob.
    if (inputs.mode == GEM_MODE_LFO_FM) {
        fm_amount = UINT12_NORMALIZE(inputs.pulse_cv_code + inputs.pulse_knob_code);
    }

    fm_amount = fix16_sub(fm_amount, GEM_FM_DEADZONE);

    if (fm_amount > F16(0)) {
        fix16_t fm = fix16_mul(osc->lfo_pitch_factor, fix16_mul(inputs.lfo_amplitude, fm_amount));
        pitch = fix16_add(pitch, fm);
    }

    // In all modes, tweak mode's pitch knobs give extra fine tuning of the
    // pitch
    if (inputs.tweak_pitch_knob_code != UINT16_MAX) {
        fix16_t fine_tune = gem_oscillator_calc_pitch_knob_(F16(-0.2), F16(0.2), 0, inputs.tweak_pitch_knob_code);
        pitch = fix16_add(pitch, fine_tune);
    }

    pitch = fix16_add(base_offset, pitch);
    pitch = fix16_clamp(pitch, F16(0), F16(7));

    osc->pitch = pitch;
}

static fix16_t gem_oscillator_calc_pitch_cv_(fix16_t cv_min, fix16_t cv_max, uint16_t adc_code) {
    // Error correction must be applied *before* inverting the code because
    // it's calibrated with the uninverted code. See
    // ./factory/libgemini/adc_calibration.py
    fix16_t cv_adc_code_f16 =
        UINT12_INVERT_F(wntr_apply_error_correction_fix16(fix16_from_int(adc_code), pitch_cv_adc_errors_));

    fix16_t cv_norm = UINT12_NORMALIZE_F(cv_adc_code_f16);
    fix16_t cv_range = fix16_sub(cv_max, cv_min);
    fix16_t cv = fix16_add(cv_min, fix16_mul(cv_norm, cv_range));

    return cv;
}

static fix16_t
gem_oscillator_calc_pitch_knob_(fix16_t knob_min, fix16_t knob_max, fix16_t nonlinearity, uint16_t adc_code) {
    // Read the pitch knob and normalize (0.0 -> 1.0) its value.
    uint16_t knob_adc_code = adc_code;
    fix16_t knob_value = fix16_div(fix16_from_int(knob_adc_code), F16(4095.0));

    // Use the range and the normalized value to determine the knob's CV value.
    fix16_t knob_range = fix16_sub(knob_max, knob_min);
    knob_value = fix16_add(knob_min, fix16_mul(knob_range, knob_value));

    // Now to get fancy: if the knob's CV value is between -1.0 and +1.0, apply
    // the bezier input transformation to make it easier to tune the oscillator
    // to values near 0.0.
    if (nonlinearity != 0 && knob_value > F16(-1.0) && knob_value < F16(1.0)) {
        fix16_t knob_bezier_input = fix16_div(fix16_add(knob_value, F16(1)), F16(2));
        knob_value = wntr_bezier_cubic_1d(
            F16(-1.0),
            fix16_add(F16(0.0), nonlinearity),
            fix16_sub(F16(0.0), nonlinearity),
            F16(1.0),
            knob_bezier_input);
    }

    return knob_value;
}

static void GemOscillator_update_pulse_width_(struct GemOscillator* osc, struct GemOscillatorInputs inputs) {
    int32_t pulse_width = 2048;

    // In normal and hard sync modes, the CV & knob directly control the pulse
    // width.
    if (inputs.mode == GEM_MODE_NORMAL || inputs.mode == GEM_MODE_HARD_SYNC) {
        pulse_width = inputs.pulse_knob_code + inputs.pulse_cv_code;
    }

    // In LFO FM mode, the tweak mode pulse knob controls the pulse width.
    else if (inputs.mode == GEM_MODE_LFO_FM) {
        if (inputs.tweak_pulse_knob_code != UINT16_MAX) {
            pulse_width = inputs.tweak_pulse_knob_code;
        }
    }

    // In LFO PWM mode, the pulse width is modulated by the LFO and the knobs
    // and CV control the amount of modulation. The center of the pulse width
    // is set by the tweak knob.
    else if (inputs.mode == GEM_MODE_LFO_PWM) {
        fix16_t lfo_factor = UINT12_NORMALIZE(inputs.pulse_knob_code + inputs.pulse_cv_code);
        if (inputs.tweak_pulse_knob_code != UINT16_MAX) {
            pulse_width = inputs.tweak_pulse_knob_code;
        }
        pulse_width +=
            fix16_to_int(fix16_mul(F16(GEM_PULSE_WIDTH_MOD_MAX), fix16_mul(lfo_factor, inputs.lfo_amplitude)));
    }

    // Up until this point, pulse width is defined as 0 -> 4095, however, the pulse width's
    // usable range is slightly lower than that.  This adjusts the value into that range.
    // Equivalent to (pulse_width * 4095) * GEM_PULSE_MAX
    pulse_width = (((pulse_width << 16) / 4095) * GEM_PULSE_WIDTH_MAX) >> 16;

    // Wrap-around
    pulse_width %= GEM_PULSE_WIDTH_MAX;

    // Apply the pulse width bitmask to emulate the old "steppy" behavior
    pulse_width = (uint16_t)(pulse_width) & (osc->pulse_width_bitmask);

    osc->pulse_width = pulse_width;
}
