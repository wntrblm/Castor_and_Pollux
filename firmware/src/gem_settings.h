#pragma once

#include "fix16.h"
#include <stdbool.h>
#include <stdint.h>

/* When adding/modifying fields be sure to update:
    _default_settings
    gem_settings_*()
    libgemini/gemini.py
*/
struct gem_settings {
    uint16_t adc_gain_corr;
    uint16_t adc_offset_corr;
    uint16_t led_brightness;
    fix16_t castor_knob_min;
    fix16_t castor_knob_max;
    fix16_t pollux_knob_min;
    fix16_t pollux_knob_max;
    fix16_t chorus_max_intensity;
    fix16_t chorus_frequency;
    fix16_t knob_offset_corr;
    fix16_t knob_gain_corr;
};

bool gem_settings_deserialize(struct gem_settings* settings, uint8_t* data);
void gem_settings_serialize(struct gem_settings* settings, uint8_t* data);
bool gem_settings_load(struct gem_settings* settings);
void gem_settings_save(struct gem_settings* settings);
void gem_settings_erase();
void gem_settings_print(struct gem_settings* settings);