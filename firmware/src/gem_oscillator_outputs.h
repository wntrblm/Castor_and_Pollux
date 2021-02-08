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

struct GemOscillatorOutputs {
    fix16_t pitch_cv;
    uint32_t period;
    uint16_t castor_ramp_cv;
    uint16_t pollux_ramp_cv;
};

void GemOscillatorOutputs_calculate(fix16_t pitch_cv, struct GemOscillatorOutputs* out);
