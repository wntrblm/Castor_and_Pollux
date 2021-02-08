/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_oscillator.h"
#include "gem_adc.h"
#include "wntr_bezier.h"

/* Static variables */

static fix16_t knob_bezier_lut_[GEM_KNOB_BEZIER_LUT_LEN];
static struct GemADCErrors pitch_cv_adc_errors_;

/* Forward declarations */

static void calculate_pitch_cv_(struct GemOscillator* osc, struct GemOscillatorInputs inputs);
static void calculate_pulse_width_(struct GemOscillator* osc, struct GemOscillatorInputs inputs);

/* Public functions */

void gem_oscillator_init(struct GemADCErrors pitch_cv_adc_errors, fix16_t pitch_knob_nonlinearity) {
    pitch_cv_adc_errors_ = pitch_cv_adc_errors;

    /* Generate the LUT table for the pitch knobs' non-linear response. */
    wntr_bezier_1d_2c_generate_lut(
        pitch_knob_nonlinearity,
        fix16_sub(F16(1.0), pitch_knob_nonlinearity),
        knob_bezier_lut_,
        GEM_KNOB_BEZIER_LUT_LEN);
}

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
    bool lfo_pwm) {

    osc->number = number;
    osc->pitch_cv_channel = pitch_cv_channel;
    osc->pitch_knob_channel = pitch_knob_channel;
    osc->pulse_width_cv_channel = pulse_width_cv_channel;
    osc->pulse_width_knob_channel = pulse_width_knob_channel;
    osc->knob_min = knob_min;
    osc->knob_range = fix16_sub(knob_max, knob_min);
    osc->follower_threshold = 0;
    osc->lfo_pwm = lfo_pwm;
    osc->lfo_pitch = false;

    osc->outputs = (struct GemOscillatorOutputs){};
    osc->smooth.initial_gain = smooth_initial_gain;
    osc->smooth.sensitivity = smooth_sensitivity;
    osc->smooth._lowpass1 = F16(0);
    osc->smooth._lowpass2 = F16(0);
    osc->pitch = F16(0);
    osc->pulse_width = 2048;
}

void GemOscillator_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs) {
    calculate_pitch_cv_(osc, inputs);
    calculate_pulse_width_(osc, inputs);
}

void GemOscillator_post_update(struct GemOscillator* osc, struct GemOscillatorInputs inputs) {
    /*
        Apply dynamic smoothing to reduce noise from the ADC inputs.
        This is done in post update so that main() can grab Castor's unfiltered
        value so if Pollux is following Castor it will be lock-step instead of
        lagging slightly behind.
    */
    osc->pitch = WntrSmoothie_step(&osc->smooth, osc->pitch);

    /* Apply LFO to pitch if its enabled for this oscillator. */
    if (osc->lfo_pitch) {
        osc->pitch = fix16_add(osc->pitch, inputs.lfo_pitch);
    }

    /* Limit the pitch value so that its with the note table. */
    osc->pitch = fix16_clamp(osc->pitch, F16(0), F16(7));

    /*
        Use the note and charge look-up tables to calculate the outputs for the
        oscillator.
    */
    GemOscillatorOutputs_calculate(osc->number, osc->pitch, &osc->outputs);
}

/* Private functions */

static void calculate_pitch_cv_(struct GemOscillator* osc, struct GemOscillatorInputs inputs) {
    /*
        The basic pitch CV determination formula is:
        1.0v + (CV in * CV_RANGE) + ((CV knob * KNOB_RANGE) - KNOB_RANGE / 2)
    */

    uint16_t cv_adc_code = inputs.adc[osc->pitch_cv_channel];

    /*
        Error correction must be applied *before* inverting the code because
        it's calibrated with the uninverted code. See
        ./factory/libgemini/adc_calibration.py
    */
    fix16_t cv_adc_code_adj = gem_adc_correct_errors(fix16_from_int(cv_adc_code), pitch_cv_adc_errors_);
    cv_adc_code_adj = fix16_sub(F16(4095), cv_adc_code_adj);

    /*
        This allows the second oscillator to follow the first. If the pitch CV
        in is below a certain threshold, then just use "osc->pitch_cv" (which
        will be set to the first oscillator's pitch CV).
    */
    if (osc->follower_threshold > 0 && cv_adc_code_adj < fix16_from_int(osc->follower_threshold)) {
        osc->pitch_cv = osc->pitch;
    }
    /*
        Otherwise, calculate the pitch CV from the input.
    */
    else {
        fix16_t cv_adc = ADC_NORMALIZE_F16(cv_adc_code_adj);
        osc->pitch_cv = fix16_add(GEM_CV_BASE_OFFSET, fix16_mul(GEM_CV_INPUT_RANGE, cv_adc));
    }

    uint16_t knob_adc_code = inputs.adc[osc->pitch_knob_channel];
    fix16_t knob_value = fix16_sub(F16(1.0), fix16_div(fix16_from_int(knob_adc_code), F16(4095.0)));

    /* Adjust the knob value using the non-linear lookup table. */
    knob_value = wntr_bezier_1d_lut_lookup(knob_bezier_lut_, GEM_KNOB_BEZIER_LUT_LEN, knob_value);

    /* Finally, multiply the normalized value by the range. */
    osc->pitch_knob = fix16_add(osc->knob_min, fix16_mul(osc->knob_range, knob_value));

    osc->pitch = fix16_add(osc->pitch_cv, osc->pitch_knob);
}

void calculate_pulse_width_(struct GemOscillator* osc, struct GemOscillatorInputs inputs) {
    osc->pulse_width_knob = ADC_INVERT(inputs.adc[osc->pulse_width_knob_channel]);
    osc->pulse_width_cv = ADC_INVERT(inputs.adc[osc->pulse_width_cv_channel]);

    /*
        The user can configure the LFO to modulate the pulse width. If it's
        enabled, the CV & knob determines the *intensity* of the modulation.
    */
    if (osc->lfo_pwm) {
        fix16_t lfo_multiplier = ADC_NORMALIZE_CODE(osc->pulse_width_cv + osc->pulse_width_knob);
        uint16_t duty_lfo =
            2048 + fix16_to_int(fix16_mul(F16(2048), fix16_mul(lfo_multiplier, inputs.lfo_pulse_width)));
        osc->pulse_width = osc->pulse_width_cv + duty_lfo;
    }
    /*
        If the user isn't using LFO modulation, then the CV & knob directly
        control the pulse width.
    */
    else {
        osc->pulse_width = osc->pulse_width_cv + osc->pulse_width_knob;
    }

    ADC_UINT12_CLAMP(osc->pulse_width);
}
