#include "gem_settings.h"
#include "gem_config.h"
#include "gem_nvm.h"
#include <stdarg.h>

#define SETTINGS_MARKER 0x66
#define SETTINGS_LEN 31

static const struct gem_settings _default_settings = {
    .adc_gain_corr = 2048,
    .adc_offset_corr = 0,
    .led_brightness = 127,
    .castor_knob_min = F16(-1.01),
    .castor_knob_max = F16(1.01),
    .pollux_knob_min = F16(-1.01),
    .pollux_knob_max = F16(1.01),
    .chorus_max_intensity = F16(0.05),
    .chorus_frequency = F16(0.2),
};

bool gem_settings_deserialize(struct gem_settings* settings, uint8_t* data) {
    /* Check for the magic flag. */
    if (data[0] != SETTINGS_MARKER) {
        (*settings) = _default_settings;
        return false;
    }

    settings->adc_gain_corr = data[1] << 8 | data[2];
    settings->adc_offset_corr = data[3] << 8 | data[4];
    settings->led_brightness = data[5] << 8 | data[6];
    settings->castor_knob_min = data[7] << 24 | data[8] << 16 | data[9] << 8 | data[10];
    settings->castor_knob_max = data[11] << 24 | data[12] << 16 | data[13] << 8 | data[14];
    settings->pollux_knob_min = data[15] << 24 | data[16] << 16 | data[17] << 8 | data[18];
    settings->pollux_knob_max = data[19] << 24 | data[20] << 16 | data[21] << 8 | data[22];
    settings->chorus_max_intensity = data[23] << 24 | data[24] << 16 | data[25] << 8 | data[26];
    settings->chorus_frequency = data[27] << 24 | data[28] << 16 | data[29] << 8 | data[30];

    return true;
}

bool gem_settings_load(struct gem_settings* settings) {
    uint8_t data[SETTINGS_LEN];
    gem_nvm_read(GEM_NVM_SETTINGS_BASE_ADDR, data, SETTINGS_LEN);
    /* TODO: Validate settings and put hard limits on certain paramters, as they can cause crashes/hangs. */
    return gem_settings_deserialize(settings, data);
}

void gem_settings_serialize(struct gem_settings* settings, uint8_t* data) {
    data[0] = SETTINGS_MARKER;
    data[1] = settings->adc_gain_corr >> 8;
    data[2] = settings->adc_gain_corr & 0xFF;
    data[3] = settings->adc_offset_corr >> 8;
    data[4] = settings->adc_offset_corr & 0xFF;
    data[5] = settings->led_brightness >> 8;
    data[6] = settings->led_brightness & 0xFF;
    data[7] = settings->castor_knob_min >> 24;
    data[8] = settings->castor_knob_min >> 16;
    data[9] = settings->castor_knob_min >> 8;
    data[10] = settings->castor_knob_min & 0xFF;
    data[11] = settings->castor_knob_max >> 24;
    data[12] = settings->castor_knob_max >> 16;
    data[13] = settings->castor_knob_max >> 8;
    data[14] = settings->castor_knob_max & 0xFF;
    data[15] = settings->pollux_knob_min >> 24;
    data[16] = settings->pollux_knob_min >> 16;
    data[17] = settings->pollux_knob_min >> 8;
    data[18] = settings->pollux_knob_min & 0xFF;
    data[19] = settings->pollux_knob_max >> 24;
    data[20] = settings->pollux_knob_max >> 16;
    data[21] = settings->pollux_knob_max >> 8;
    data[22] = settings->pollux_knob_max & 0xFF;
    data[23] = settings->chorus_max_intensity >> 24;
    data[24] = settings->chorus_max_intensity >> 16;
    data[25] = settings->chorus_max_intensity >> 8;
    data[26] = settings->chorus_max_intensity & 0xFF;
    data[27] = settings->chorus_frequency >> 24;
    data[28] = settings->chorus_frequency >> 16;
    data[29] = settings->chorus_frequency >> 8;
    data[30] = settings->chorus_frequency & 0xFF;
};

void gem_settings_save(struct gem_settings* settings) {
    uint8_t data[SETTINGS_LEN];
    gem_settings_serialize(settings, data);
    gem_nvm_write(GEM_NVM_SETTINGS_BASE_ADDR, data, SETTINGS_LEN);
}

void gem_settings_erase() {
    uint8_t data[1] = {0xFF};
    gem_nvm_write(GEM_NVM_SETTINGS_BASE_ADDR, data, 1);
}