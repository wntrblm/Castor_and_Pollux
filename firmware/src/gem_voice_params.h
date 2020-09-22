#pragma once

#include <stddef.h>
#include <stdint.h>
#include <fix16.h>

/* This defines the structure and functions used
   to map input CV to the required frequency and charge voltage.
   Frequency is configured by the PWM period register and the charge
   voltage is configured by the DAC code.
*/

struct gem_voice_params {
    fix16_t voltage;
    uint32_t period_reg;
    uint16_t castor_dac_code;
    uint16_t pollux_dac_code;
};

void gem_voice_params_from_cv(
    struct gem_voice_params* table, size_t table_len, fix16_t voltage, struct gem_voice_params* out);