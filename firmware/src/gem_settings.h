#pragma once

/*
    Hey this file was generated by Structy on 2021-01-14 04:19:08.966871 from
    gem_settings.structy. Maybe don't edit it!
*/

#include "structy.h"

#include "fix16.h"

#define GEMSETTINGS_PACKED_SIZE 54

struct GemSettings {
    /* The ADC's internal gain correction register. */
    uint16_t adc_gain_corr;
    /* The ADC's internal offset correction register. */
    int16_t adc_offset_corr;
    /* The front-plate LED brightness. */
    uint16_t led_brightness;
    /* Configuration for the CV knob mins and maxs in volts, defaults to -1.01 to +1.01. */
    fix16_t castor_knob_min;
    fix16_t castor_knob_max;
    fix16_t pollux_knob_min;
    fix16_t pollux_knob_max;
    /* Maximum amount that the chorus can impact Pollux's frequency. */
    fix16_t chorus_max_intensity;
    /* The default LFO frequency in hertz. */
    fix16_t lfo_frequency;
    /* Error correction for the ADC readings for the CV and PWM knobs. */
    fix16_t knob_offset_corr;
    fix16_t knob_gain_corr;
    /* Pitch input CV smoothing parameters. */
    fix16_t smooth_initial_gain;
    fix16_t smooth_sensitivity;
    /* This is the "deadzone" for Pollux's pitch CV input, basically, it
       should be around 0v and it's the point where Pollux starts following
       Castor's pitch CV. By default this is 6 code points to allow some
       variance in time and temperature. */
    uint16_t pollux_follower_threshold;
    /* Route LFO to PWM for oscillators */
    bool castor_lfo_pwm;
    bool pollux_lfo_pwm;
    /* The firmware adjusts the pitch CV knobs so that it's easier to tune
       the oscillators. It does this by spreading the values near the center
       of the knob out so that the range at the center is more fine. This
       makes the knobs non-linear. This setting controls how "intense"
       this non-linearity is. A value of 0.33 is equivalent to a linear
       response. Higher values make it easier and easier to tune the
       oscillator but sacrifice the range, values lower than 0.33 will
       make it harder to tune and aren't recommended. */
    fix16_t pitch_knob_nonlinearity;
};

void GemSettings_init(struct GemSettings* inst);
struct StructyResult GemSettings_pack(const struct GemSettings* inst, uint8_t* buf);
struct StructyResult GemSettings_unpack(struct GemSettings* inst, const uint8_t* buf);
void GemSettings_print(const struct GemSettings* inst);
