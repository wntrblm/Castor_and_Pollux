/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

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
    uint32_t period;
    uint16_t castor;
    uint16_t pollux;
};

struct GemVoiceParams {
    struct GemVoltageAndPeriod voltage_and_period;
    struct GemDACCodePair dac_codes;
};

void GemVoiceParams_from_cv(fix16_t voltage, struct GemVoiceParams* out);
