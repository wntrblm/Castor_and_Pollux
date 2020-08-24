#include "gem_config.h"
#include "gem_nvm.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#endif

#define NVM_SETTINGS_MARKER 0x63

const struct gem_adc_input gem_adc_inputs[] = {
    {1, PORT_PB09, ADC_INPUTCTRL_MUXPOS_PIN3},
    {1, PORT_PB08, ADC_INPUTCTRL_MUXPOS_PIN2},
};

void gem_config_init() {}


bool gem_config_get_nvm_settings(struct gem_nvm_settings* settings) {
    uint8_t data[7];

    /* Check for the magic flag. */
    gem_nvm_read(GEM_NVM_SETTINGS_BASE_ADDR, data, 7);

    if(data[0] != NVM_SETTINGS_MARKER) {
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