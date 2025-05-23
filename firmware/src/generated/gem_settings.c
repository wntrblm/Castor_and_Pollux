/*
    Hey this file was generated by Structy on from
    gem_settings.structy. Maybe don't edit it!
*/

#include "gem_settings.h"

#define _PACK_STRING "HhHiiiiiiiiiiH??iiiBBiiHI??"

void GemSettings_init(struct GemSettings* inst) {
    inst->adc_gain_corr = 2048;
    inst->adc_offset_corr = 0;
    inst->led_brightness = 127;
    inst->castor_knob_min = F16(-1.2);
    inst->castor_knob_max = F16(1.2);
    inst->pollux_knob_min = F16(-1.2);
    inst->pollux_knob_max = F16(1.2);
    inst->chorus_max_intensity = F16(0.05);
    inst->lfo_1_frequency = F16(0.2);
    inst->cv_offset_error = F16(0.0);
    inst->cv_gain_error = F16(1.0);
    inst->removed_smooth_initial_gain = F16(0.1);
    inst->removed_smooth_sensitivity = F16(30.0);
    inst->zero_detection_threshold = 350;
    inst->removed_castor_lfo_pwm = false;
    inst->removed_pollux_lfo_pwm = false;
    inst->pitch_knob_nonlinearity = F16(0.6);
    inst->base_cv_offset = F16(1.0);
    inst->lfo_2_frequency_ratio = F16(2);
    inst->lfo_1_waveshape = 0;
    inst->lfo_2_waveshape = 0;
    inst->lfo_1_factor = F16(1);
    inst->lfo_2_factor = F16(0);
    inst->pulse_width_bitmask = 4095;
    inst->osc8m_freq = 8000000;
    inst->zero_detection_enabled = true;
    inst->quantization_enabled = true;
}

struct StructyResult GemSettings_pack(const struct GemSettings* inst, uint8_t* buf) {
    return structy_pack(
        _PACK_STRING,
        buf,
        GEMSETTINGS_PACKED_SIZE,
        inst->adc_gain_corr,
        inst->adc_offset_corr,
        inst->led_brightness,
        inst->castor_knob_min,
        inst->castor_knob_max,
        inst->pollux_knob_min,
        inst->pollux_knob_max,
        inst->chorus_max_intensity,
        inst->lfo_1_frequency,
        inst->cv_offset_error,
        inst->cv_gain_error,
        inst->removed_smooth_initial_gain,
        inst->removed_smooth_sensitivity,
        inst->zero_detection_threshold,
        inst->removed_castor_lfo_pwm,
        inst->removed_pollux_lfo_pwm,
        inst->pitch_knob_nonlinearity,
        inst->base_cv_offset,
        inst->lfo_2_frequency_ratio,
        inst->lfo_1_waveshape,
        inst->lfo_2_waveshape,
        inst->lfo_1_factor,
        inst->lfo_2_factor,
        inst->pulse_width_bitmask,
        inst->osc8m_freq,
        inst->zero_detection_enabled,
        inst->quantization_enabled);
}

struct StructyResult GemSettings_unpack(struct GemSettings* inst, const uint8_t* buf) {
    return structy_unpack(
        _PACK_STRING,
        buf,
        GEMSETTINGS_PACKED_SIZE,
        &inst->adc_gain_corr,
        &inst->adc_offset_corr,
        &inst->led_brightness,
        &inst->castor_knob_min,
        &inst->castor_knob_max,
        &inst->pollux_knob_min,
        &inst->pollux_knob_max,
        &inst->chorus_max_intensity,
        &inst->lfo_1_frequency,
        &inst->cv_offset_error,
        &inst->cv_gain_error,
        &inst->removed_smooth_initial_gain,
        &inst->removed_smooth_sensitivity,
        &inst->zero_detection_threshold,
        &inst->removed_castor_lfo_pwm,
        &inst->removed_pollux_lfo_pwm,
        &inst->pitch_knob_nonlinearity,
        &inst->base_cv_offset,
        &inst->lfo_2_frequency_ratio,
        &inst->lfo_1_waveshape,
        &inst->lfo_2_waveshape,
        &inst->lfo_1_factor,
        &inst->lfo_2_factor,
        &inst->pulse_width_bitmask,
        &inst->osc8m_freq,
        &inst->zero_detection_enabled,
        &inst->quantization_enabled);
}

void GemSettings_print(const struct GemSettings* inst) {
    // TODO: Print pointer address.
    STRUCTY_PRINTF("Struct GemSettings:\n");
    STRUCTY_PRINTF("- adc_gain_corr: %u\n", inst->adc_gain_corr);
    STRUCTY_PRINTF("- adc_offset_corr: %d\n", inst->adc_offset_corr);
    STRUCTY_PRINTF("- led_brightness: %u\n", inst->led_brightness);
    {
        char fix16buf[13];
        fix16_to_str(inst->castor_knob_min, fix16buf, 2);
        STRUCTY_PRINTF("- castor_knob_min: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->castor_knob_max, fix16buf, 2);
        STRUCTY_PRINTF("- castor_knob_max: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->pollux_knob_min, fix16buf, 2);
        STRUCTY_PRINTF("- pollux_knob_min: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->pollux_knob_max, fix16buf, 2);
        STRUCTY_PRINTF("- pollux_knob_max: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->chorus_max_intensity, fix16buf, 2);
        STRUCTY_PRINTF("- chorus_max_intensity: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->lfo_1_frequency, fix16buf, 2);
        STRUCTY_PRINTF("- lfo_1_frequency: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->cv_offset_error, fix16buf, 2);
        STRUCTY_PRINTF("- cv_offset_error: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->cv_gain_error, fix16buf, 2);
        STRUCTY_PRINTF("- cv_gain_error: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->removed_smooth_initial_gain, fix16buf, 2);
        STRUCTY_PRINTF("- removed_smooth_initial_gain: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->removed_smooth_sensitivity, fix16buf, 2);
        STRUCTY_PRINTF("- removed_smooth_sensitivity: %s\n", fix16buf);
    }
    STRUCTY_PRINTF("- zero_detection_threshold: %u\n", inst->zero_detection_threshold);
    STRUCTY_PRINTF("- removed_castor_lfo_pwm: %u\n", inst->removed_castor_lfo_pwm);
    STRUCTY_PRINTF("- removed_pollux_lfo_pwm: %u\n", inst->removed_pollux_lfo_pwm);
    {
        char fix16buf[13];
        fix16_to_str(inst->pitch_knob_nonlinearity, fix16buf, 2);
        STRUCTY_PRINTF("- pitch_knob_nonlinearity: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->base_cv_offset, fix16buf, 2);
        STRUCTY_PRINTF("- base_cv_offset: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->lfo_2_frequency_ratio, fix16buf, 2);
        STRUCTY_PRINTF("- lfo_2_frequency_ratio: %s\n", fix16buf);
    }
    STRUCTY_PRINTF("- lfo_1_waveshape: %u\n", inst->lfo_1_waveshape);
    STRUCTY_PRINTF("- lfo_2_waveshape: %u\n", inst->lfo_2_waveshape);
    {
        char fix16buf[13];
        fix16_to_str(inst->lfo_1_factor, fix16buf, 2);
        STRUCTY_PRINTF("- lfo_1_factor: %s\n", fix16buf);
    }
    {
        char fix16buf[13];
        fix16_to_str(inst->lfo_2_factor, fix16buf, 2);
        STRUCTY_PRINTF("- lfo_2_factor: %s\n", fix16buf);
    }
    STRUCTY_PRINTF("- pulse_width_bitmask: %u\n", inst->pulse_width_bitmask);
    STRUCTY_PRINTF("- osc8m_freq: %u\n", inst->osc8m_freq);
    STRUCTY_PRINTF("- zero_detection_enabled: %u\n", inst->zero_detection_enabled);
    STRUCTY_PRINTF("- quantization_enabled: %u\n", inst->quantization_enabled);
}
