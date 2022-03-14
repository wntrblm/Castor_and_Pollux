/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_oscillator.h"
#include "gem_config.h"
#include "wntr_bezier.h"
#include "wntr_uint12.h"

/* Static variables */

static fix16_t pitch_knob_nonlinearity_;
static struct WntrErrorCorrection pitch_cv_adc_errors_;

/* Forward declarations */

static void calculate_pitch_cv_(struct GemOscillator* osc, struct GemOscillatorInputs inputs);
static void calculate_pulse_width_(struct GemOscillator* osc, struct GemOscillatorInputs inputs);

/* Public functions */

void gem_oscillator_init(struct WntrErrorCorrection pitch_cv_adc_error_correction, fix16_t pitch_knob_nonlinearity) {
    pitch_cv_adc_errors_ = pitch_cv_adc_error_correction;
    pitch_knob_nonlinearity_ = pitch_knob_nonlinearity;
}

void GemOscillator_init(
    struct GemOscillator* osc,
    uint8_t number,
    enum GemADCChannel pitch_cv_channel,
    enum GemADCChannel pitch_knob_channel,
    enum GemADCChannel pulse_width_cv_channel,
    enum GemADCChannel pulse_width_knob_channel,
    fix16_t smooth_initial_gain,
    fix16_t smooth_sensitivity,
    fix16_t base_offset,
    fix16_t knob_min,
    fix16_t knob_max,
    bool lfo_pwm,
    bool quantize) {

    osc->number = number;
    osc->pitch_cv_channel = pitch_cv_channel;
    osc->pitch_knob_channel = pitch_knob_channel;
    osc->pulse_width_cv_channel = pulse_width_cv_channel;
    osc->pulse_width_knob_channel = pulse_width_knob_channel;
    osc->base_offset = base_offset;
    osc->knob_min = knob_min;
    osc->knob_range = fix16_sub(knob_max, knob_min);
    osc->follower_threshold = 0;
    osc->lfo_pwm = lfo_pwm;
    osc->lfo_pitch = false;
    osc->quantize = quantize;

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

        This is not done if quantization is enabled since it would just introduce
        unnecessary latency.
    */
    if (!osc->quantize) {
        osc->pitch = WntrSmoothie_step(&osc->smooth, osc->pitch);
    }

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
        To determine pitch, we sum two terms:
            * CV knob (never quantized)
                (CV knob * KNOB_RANGE) - KNOB_RANGE / 2
            * Pitch CV input (quantized if enabled in the settings)
                quantize(base_cv_offset + (CV in * CV_RANGE))

        Note that base_cv_offset is included in the quantized part.
        This is so that it is possible to calibrate against an external
        CV source, so that the pitch CVs it produces get mapped to the
        middle of the range for each note, for maximum noise tolerance.

        Without this, we could encounter a nasty edge case where the CVs
        land right near the boundary between two notes, causing the quantizer
        to flip back and forth between two adjacent notes.
    */

    uint16_t cv_adc_code = inputs.adc[osc->pitch_cv_channel];

    /*
        Error correction must be applied *before* inverting the code because
        it's calibrated with the uninverted code. See
        ./factory/libgemini/adc_calibration.py
    */
    fix16_t cv_adc_code_f16 =
        UINT12_INVERT_F(wntr_apply_error_correction_fix16(fix16_from_int(cv_adc_code), pitch_cv_adc_errors_));

    /*
        This allows the second oscillator to follow the first. If the pitch CV
        in is below a certain threshold, then just use "osc->pitch_cv" (which
        will be set to the first oscillator's pitch CV).
    */
    if (osc->follower_threshold > 0 && cv_adc_code_f16 < fix16_from_int(osc->follower_threshold)) {
        osc->pitch_cv = osc->pitch;
    }
    /*
        Otherwise, calculate the pitch CV from the input.
    */
    else {
        fix16_t cv = UINT12_NORMALIZE_F(cv_adc_code_f16);
        fix16_t pitch_cv = fix16_add(osc->base_offset, fix16_mul(GEM_CV_INPUT_RANGE, cv));

        if (osc->quantize) {
            /*
              Quantize to the nearest 12-tone equal temperament note.
            */
            pitch_cv = pitch_cv * 12;
            pitch_cv = fix16_floor(fix16_add(pitch_cv, F16(0.5)));
            pitch_cv = (pitch_cv + 6) / 12;
        }

        osc->pitch_cv = pitch_cv;
    }

    /* Read the pitch knob and normalize (0.0 -> 1.0) its value. */
    uint16_t knob_adc_code = inputs.adc[osc->pitch_knob_channel];
    fix16_t knob_value = fix16_sub(F16(1.0), fix16_div(fix16_from_int(knob_adc_code), F16(4095.0)));

    /* Use the range and the normalized value to determine the knob's CV value. */
    knob_value = fix16_add(osc->knob_min, fix16_mul(osc->knob_range, knob_value));

    /*
        Now to get fancy: if the knob's CV value is between -1.0 and +1.0, apply
        the bezier input transformation to make it easier to tune the oscillator
        to values near 0.0.
    */
    if (knob_value > F16(-1.0) && knob_value < F16(1.0)) {
        fix16_t knob_bezier_input = fix16_div(fix16_add(knob_value, F16(1)), F16(2));
        knob_value = wntr_bezier_cubic_1d(
            F16(-1.0),
            fix16_add(F16(0.0), pitch_knob_nonlinearity_),
            fix16_sub(F16(0.0), pitch_knob_nonlinearity_),
            F16(1.0),
            knob_bezier_input);
    }

    osc->pitch_knob = knob_value;

    osc->pitch = fix16_add(osc->pitch_cv, osc->pitch_knob);
}

void calculate_pulse_width_(struct GemOscillator* osc, struct GemOscillatorInputs inputs) {
    osc->pulse_width_knob = UINT12_INVERT(inputs.adc[osc->pulse_width_knob_channel]);
    osc->pulse_width_cv = UINT12_INVERT(inputs.adc[osc->pulse_width_cv_channel]);

    /*
        The user can configure the LFO to modulate the pulse width. If it's
        enabled, the CV & knob determines the *intensity* of the modulation.
    */
    if (osc->lfo_pwm) {
        fix16_t lfo_multiplier = UINT12_NORMALIZE(osc->pulse_width_cv + osc->pulse_width_knob);
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

    UINT12_CLAMP(osc->pulse_width);
}
