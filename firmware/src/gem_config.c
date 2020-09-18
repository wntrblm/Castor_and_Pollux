#include "gem_config.h"
#include "gem_nvm.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#endif

#define NVM_SETTINGS_MARKER 0x63

const struct gem_nvm_settings gem_default_nvm_settings = {
    .adc_gain_corr = 2048,
    .adc_offset_corr = 0,
    .led_brightness = 127,
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

bool gem_config_get_nvm_settings(struct gem_nvm_settings* settings) {
    uint8_t data[7];

    /* Check for the magic flag. */
    gem_nvm_read(GEM_NVM_SETTINGS_BASE_ADDR, data, 7);

    if (data[0] != NVM_SETTINGS_MARKER) {
        (*settings) = gem_default_nvm_settings;
        return false;
    }

    settings->adc_gain_corr = data[1] << 8 | data[2];
    settings->adc_offset_corr = data[3] << 8 | data[4];
    settings->led_brightness = data[5] << 8 | data[6];

    return true;
}

void gem_config_save_nvm_settings(struct gem_nvm_settings* settings) {
    uint8_t data[7] = {
        NVM_SETTINGS_MARKER,
        settings->adc_gain_corr >> 8,
        settings->adc_gain_corr & 0xFF,
        settings->adc_offset_corr >> 8,
        settings->adc_offset_corr & 0xFF,
        settings->led_brightness >> 8,
        settings->led_brightness & 0xFF,
    };

    gem_nvm_write(GEM_NVM_SETTINGS_BASE_ADDR, data, 7);
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