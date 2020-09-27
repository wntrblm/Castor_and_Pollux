#include "gem_config.h"
#include "gem_nvm.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#endif

#define NVM_SETTINGS_MARKER 0x66
#define NVM_SETTINGS_LEN 31

const struct gem_nvm_settings gem_default_nvm_settings = {
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

const struct gem_adc_input gem_adc_inputs[] = {
    {0, 6, ADC_INPUTCTRL_MUXPOS_PIN6},   // CV A
    {1, 3, ADC_INPUTCTRL_MUXPOS_PIN11},  // CV A Pot
    {0, 7, ADC_INPUTCTRL_MUXPOS_PIN7},   // CV B
    {1, 2, ADC_INPUTCTRL_MUXPOS_PIN10},  // CV B Pot
    {0, 5, ADC_INPUTCTRL_MUXPOS_PIN5},   // Duty A
    {0, 8, ADC_INPUTCTRL_MUXPOS_PIN16},  // Duty A Pot
    {1, 9, ADC_INPUTCTRL_MUXPOS_PIN3},   // Duty B
    {0, 9, ADC_INPUTCTRL_MUXPOS_PIN17},  // Duty B Pot
    {0, 2, ADC_INPUTCTRL_MUXPOS_PIN0},   // Chorus pot
};

void gem_config_init() {}

bool gem_config_deserialize_nvm_settings(struct gem_nvm_settings* settings, uint8_t* data) {
    /* Check for the magic flag. */
    if (data[0] != NVM_SETTINGS_MARKER) {
        (*settings) = gem_default_nvm_settings;
        return false;
    }

    settings->adc_gain_corr = data[1] << 8 | data[2];
    settings->adc_offset_corr = data[3] << 8 | data[4];
    settings->led_brightness = data[5] << 8 | data[6];
    settings->castor_knob_min = data[7] << 24 | data[8] << 16 | data[9] << 8 | data[10];
    settings->castor_knob_max = data[11] << 24 | data[12] << 16 | data[13] << 8 | data[14];
    settings->castor_knob_min = data[15] << 24 | data[16] << 16 | data[17] << 8 | data[18];
    settings->castor_knob_max = data[19] << 24 | data[20] << 16 | data[21] << 8 | data[22];
    settings->chorus_max_intensity = data[23] << 24 | data[24] << 16 | data[25] << 8 | data[26];
    settings->chorus_frequency = data[27] << 24 | data[28] << 16 | data[29] << 8 | data[30];

    return true;
}

bool gem_config_get_nvm_settings(struct gem_nvm_settings* settings) {
    uint8_t data[NVM_SETTINGS_LEN];
    gem_nvm_read(GEM_NVM_SETTINGS_BASE_ADDR, data, NVM_SETTINGS_LEN);
    return gem_config_deserialize_nvm_settings(settings, data);
}

void gem_config_serialize_nvm_settings(struct gem_nvm_settings* settings, uint8_t* data) {
    data[0] = NVM_SETTINGS_MARKER;
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

void gem_config_save_nvm_settings(struct gem_nvm_settings* settings) {
    uint8_t data[NVM_SETTINGS_LEN];
    gem_config_serialize_nvm_settings(settings, data);
    gem_nvm_write(GEM_NVM_SETTINGS_BASE_ADDR, data, NVM_SETTINGS_LEN);
}

void gem_config_erase_nvm_settings() {
    uint8_t data[1] = {0xFF};
    gem_nvm_write(GEM_NVM_SETTINGS_BASE_ADDR, data, 1);
}

int __wrap_printf(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    int result = SEGGER_RTT_vprintf(0, format, &args);
    va_end(args);
    return result;
#else
    (void)format;
    return 0;
#endif
}