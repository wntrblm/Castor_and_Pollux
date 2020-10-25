#pragma once

/*
    Structures and functions used to map input CV to the required frequency
    and charge voltage. Frequency is configured by the PWM period register
    and the charge voltage is configured by the DAC code.
*/

#include "fix16.h"
#include <stddef.h>
#include <stdint.h>

/* The configuration values for this struct are generated ahead of time and are static,
   so they're a separate struct. */
struct gem_voice_voltage_and_period {
    fix16_t voltage;
    uint32_t period;
};

/* The configuration values for this struct are calibrated after assembly, therefore
   they are stored in NVM so they're a separate struct. */
struct gem_voice_dac_codes {
    uint16_t castor;
    uint16_t pollux;
};

struct gem_voice_params {
    struct gem_voice_voltage_and_period voltage_and_period;
    struct gem_voice_dac_codes dac_codes;
};

void gem_voice_params_from_cv(
    const struct gem_voice_voltage_and_period* volt_table,
    struct gem_voice_dac_codes* dac_table,
    size_t table_len,
    fix16_t voltage,
    struct gem_voice_params* out);