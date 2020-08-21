#pragma once

#include <stddef.h>
#include <stdint.h>

/* This defines the structure and functions used
   to map input CV to the required frequency and charge voltage.
   Frequency is configured by the PWM period register and the charge
   voltage is configured by the DAC code.
*/

struct gem_voice_params {
    uint16_t adc_code;
    uint16_t period_reg;
    uint16_t dac_code;
};

void gem_voice_params_from_adc_code(struct gem_voice_params* table,
                                    size_t table_len,
                                    uint16_t adc_code,
                                    struct gem_voice_params* out);