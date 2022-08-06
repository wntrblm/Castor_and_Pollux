/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for using the SAM D21 TCC peripheral to generate pulse/
    square wave/pwm signals.
*/

#include "wntr_gpio.h"
#include "wntr_ramfunc.h"
#include <stdbool.h>
#include <stdint.h>

struct GemPulseOutConfig {
    uint32_t gclk;
    uint32_t gclk_div;
    struct WntrGPIOPin tcc0_pin;
    uint32_t tcc0_wo;
    struct WntrGPIOPin tcc1_pin;
    uint32_t tcc1_wo;
};

void gem_pulseout_init(const struct GemPulseOutConfig* po);

void gem_pulseout_set_period(const struct GemPulseOutConfig* po, uint8_t channel, uint32_t period) RAMFUNC;

void gem_pulseout_hard_sync(bool state) RAMFUNC;
