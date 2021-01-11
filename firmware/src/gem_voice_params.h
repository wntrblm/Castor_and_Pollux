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
struct GemVoltageAndPeriod {
    fix16_t voltage;
    uint32_t period;
};

/* The configuration values for this struct are calibrated after assembly, therefore
   they are stored in NVM so they're a separate struct. */
struct GemDACCodePair {
    uint16_t castor;
    uint16_t pollux;
};

struct GemVoiceParams {
    struct GemVoltageAndPeriod voltage_and_period;
    struct GemDACCodePair dac_codes;
};

void GemVoiceParams_from_cv(
    const struct GemVoltageAndPeriod* volt_table,
    struct GemDACCodePair* dac_table,
    size_t table_len,
    fix16_t voltage,
    struct GemVoiceParams* out);
