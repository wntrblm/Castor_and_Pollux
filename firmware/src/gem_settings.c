#include "gem_settings.h"
#include "gem_nvm.h"
#include <stdarg.h>

#define SETTINGS_MARKER 0x66
#define SETTINGS_LEN 39

#define UNPACK_16(data, idx) data[idx] << 8 | data[idx + 1]
#define UNPACK_32(data, idx) data[idx] << 24 | data[idx + 1] << 16 | data[idx + 2] << 8 | data[idx + 3]
#define PACK_16(val, data, idx)                                                                                        \
    data[idx] = val >> 8;                                                                                              \
    data[idx + 1] = val & 0xFF;
#define PACK_32(val, data, idx)                                                                                        \
    data[idx] = val >> 24;                                                                                             \
    data[idx + 1] = val >> 16;                                                                                         \
    data[idx + 2] = val >> 8;                                                                                          \
    data[idx + 3] = val & 0xFF;

extern uint8_t _nvm_settings_base_address;

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
    .knob_offset_corr = F16(0.0),
    .knob_gain_corr = F16(1.0),
};

bool gem_settings_deserialize(struct gem_settings* settings, uint8_t* data) {
    /* Check for the magic flag. */
    if (data[0] != SETTINGS_MARKER) {
        (*settings) = _default_settings;
        return false;
    }

    settings->adc_gain_corr = UNPACK_16(data, 1);
    settings->adc_offset_corr = UNPACK_16(data, 3);
    settings->led_brightness = UNPACK_16(data, 5);
    settings->castor_knob_min = UNPACK_32(data, 7);
    settings->castor_knob_max = UNPACK_32(data, 11);
    settings->pollux_knob_min = UNPACK_32(data, 15);
    settings->pollux_knob_max = UNPACK_32(data, 19);
    settings->chorus_max_intensity = UNPACK_32(data, 23);
    settings->chorus_frequency = UNPACK_32(data, 27);
    settings->knob_offset_corr = UNPACK_32(data, 31);
    settings->knob_gain_corr = UNPACK_32(data, 35);

    return true;
}

bool gem_settings_load(struct gem_settings* settings) {
    uint8_t data[SETTINGS_LEN];
    gem_nvm_read((uint32_t)(&_nvm_settings_base_address), data, SETTINGS_LEN);
    /* TODO: Validate settings and put hard limits on certain paramters, as they can cause crashes/hangs. */
    return gem_settings_deserialize(settings, data);
}

void gem_settings_serialize(struct gem_settings* settings, uint8_t* data) {
    data[0] = SETTINGS_MARKER;
    PACK_16(settings->adc_gain_corr, data, 1);
    PACK_16(settings->adc_offset_corr, data, 3);
    PACK_16(settings->led_brightness, data, 5);
    PACK_32(settings->castor_knob_min, data, 7);
    PACK_32(settings->castor_knob_max, data, 11);
    PACK_32(settings->pollux_knob_min, data, 15);
    PACK_32(settings->pollux_knob_max, data, 19);
    PACK_32(settings->chorus_max_intensity, data, 23);
    PACK_32(settings->chorus_frequency, data, 27);
    PACK_32(settings->knob_offset_corr, data, 31);
    PACK_32(settings->knob_gain_corr, data, 35);
};

void gem_settings_save(struct gem_settings* settings) {
    uint8_t data[SETTINGS_LEN];
    gem_settings_serialize(settings, data);
    gem_nvm_write((uint32_t)(&_nvm_settings_base_address), data, SETTINGS_LEN);
}

void gem_settings_erase() {
    uint8_t data[1] = {0xFF};
    gem_nvm_write((uint32_t)(&_nvm_settings_base_address), data, 1);
}