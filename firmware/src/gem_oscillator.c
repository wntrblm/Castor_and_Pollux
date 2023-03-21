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

/* Static variables */

static fix16_t pitch_knob_nonlinearity_;
static struct WntrErrorCorrection pitch_cv_adc_errors_;

/* Forward declarations */

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
    fix16_t pitch;
    bool zero = UINT12_INVERT(inputs.pitch_cv_code) < osc->nonzero_threshold;

    // "coarse" pitch mode is used when the pitch jack isn't connected to Castor
    if (osc->number == 0 && zero) {
        pitch = gem_oscillator_calc_pitch_knob(F16(0), F16(6), 0, inputs.pitch_knob_code);

        // quantize
        pitch = fix16_mul(pitch, F16(12));
        pitch = fix16_floor(fix16_add(pitch, F16(0.5)));
        pitch = fix16_div(pitch, F16(12));

        pitch = fix16_add(osc->pitch_offset, pitch);
    }

    // "follow" mode is used by Pollux when both Castor & Pollux don't have any
    // pitch CV patched.
    // pitch = reference pitch + knob
    else if (osc->number == 1 && zero) {
        pitch = fix16_add(
            inputs.reference_pitch,
            gem_oscillator_calc_pitch_knob(
                osc->pitch_knob_min, osc->pitch_knob_max, pitch_knob_nonlinearity_, inputs.pitch_knob_code));
    }

    // "fine" pitch mode is used when the pitch jack is connected to a CV source.
    // pitch = base offset + pitch CV + pitch knob
    else {
        pitch = gem_oscillator_calc_pitch_cv(osc->pitch_cv_min, osc->pitch_cv_max, inputs.pitch_cv_code);
        pitch = fix16_add(
            pitch,
            gem_oscillator_calc_pitch_knob(
                osc->pitch_knob_min, osc->pitch_knob_max, pitch_knob_nonlinearity_, inputs.pitch_knob_code));
    }

    if (osc->apply_lfo_to_pitch) {
        fix16_t fm = fix16_mul(osc->lfo_pitch_factor, fix16_mul(inputs.lfo_value, inputs.lfo_amplitude));
        pitch = fix16_add(pitch, fm);
    }

    osc->pitch = fix16_clamp(pitch, F16(0), F16(7));

    // Pulse width is much easier
    osc->pulse_width = gem_oscillator_calculate_pulse_width(
        inputs.pulse_cv_code,
        inputs.pulse_knob_code,
        osc->apply_lfo_to_pulse,
        inputs.lfo_value,
        osc->pulse_width_bitmask);
}

void GemOscillator_post_update(const struct GemPulseOutConfig* pulseout, struct GemOscillator* osc) {
    // Use the note and charge look-up tables to calculate the outputs for the
    // oscillator.
    fix16_t freq_hz = gem_voct_to_frequency(osc->pitch);
    uint64_t freq_millihz = gem_frequency_to_millihertz_f16_u64(freq_hz);

    osc->pulseout_period = gem_pulseout_frequency_to_period(pulseout, freq_millihz);
    osc->ramp_cv = gem_ramp_table_lookup(osc->number, osc->pitch);
}

fix16_t gem_oscillator_calc_pitch_cv(fix16_t cv_min, fix16_t cv_max, uint16_t adc_code) {
    /*
        Error correction must be applied *before* inverting the code because
        it's calibrated with the uninverted code. See
        ./factory/libgemini/adc_calibration.py
    */
    fix16_t cv_adc_code_f16 =
        UINT12_INVERT_F(wntr_apply_error_correction_fix16(fix16_from_int(adc_code), pitch_cv_adc_errors_));

    fix16_t cv_norm = UINT12_NORMALIZE_F(cv_adc_code_f16);
    fix16_t cv_range = fix16_sub(cv_max, cv_min);
    fix16_t cv = fix16_add(cv_min, fix16_mul(cv_norm, cv_range));

    return cv;
}

fix16_t gem_oscillator_calc_pitch_knob(fix16_t knob_min, fix16_t knob_max, fix16_t nonlinearity, uint16_t adc_code) {
    /* Read the pitch knob and normalize (0.0 -> 1.0) its value. */
    uint16_t knob_adc_code = adc_code;
    fix16_t knob_value = fix16_div(fix16_from_int(knob_adc_code), F16(4095.0));

    /* Use the range and the normalized value to determine the knob's CV value. */
    fix16_t knob_range = fix16_sub(knob_max, knob_min);
    knob_value = fix16_add(knob_min, fix16_mul(knob_range, knob_value));

    /*
        Now to get fancy: if the knob's CV value is between -1.0 and +1.0, apply
        the bezier input transformation to make it easier to tune the oscillator
        to values near 0.0.
    */
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

fix16_t gem_oscillator_calc_pitch(fix16_t base_offset, fix16_t pitch_cv, fix16_t pitch_knob) {
    /*
        The basic pitch CV determination formula is:
        (base offset) + (CV in * CV_RANGE) + ((CV knob * KNOB_RANGE) - KNOB_RANGE / 2)
    */
    return fix16_add(base_offset, fix16_add(pitch_cv, pitch_knob));
}

uint16_t gem_oscillator_calculate_pulse_width(
    uint16_t cv_adc_code, uint16_t knob_adc_code, bool apply_lfo, fix16_t lfo, uint16_t bitmask) {
    uint16_t pulse_width;

    // If the user isn't using LFO modulation, then the CV & knob directly
    // control the pulse width.
    if (!apply_lfo) {
        pulse_width = cv_adc_code + knob_adc_code;
    }
    // The user can configure the LFO to modulate the pulse width. If it's
    // enabled, the CV & knob determines the *intensity* of the modulation.
    else {
        fix16_t lfo_factor = UINT12_NORMALIZE(cv_adc_code + knob_adc_code);
        uint16_t duty_lfo = 2048 + fix16_to_int(fix16_mul(F16(2048), fix16_mul(lfo_factor, lfo)));
        pulse_width = duty_lfo;
    }

    UINT12_MOD(pulse_width);

    // Apply the pulse width bitmask to emulate the old "steppy" behavior
    pulse_width &= bitmask;

    return pulse_width;
}
