/*
    Copyright (c) 2023 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "gem_adc.h"
#include "sam.h"
#include "wntr_gpio.h"
#include <stdint.h>

/*
    With the below settings the total time per ADC conversion is
    24.67 microseconds.

    With Gemini's 9 channels it takes at least 198 microseconds to measure
    all channels. There's a bit of added overhead because the CPU has to switch
    channels between each measurement. This means that the effective sample
    rate for Gemini is about 5 kHz.

    See: https://blog.thea.codes/getting-the-most-out-of-the-samd21-adc/
*/
static const struct GemADCConfig GEM_ADC_CFG = {
    // ADC clock can be *at most* 2,100 kHz.
    // Gemini has GCLK0 @ 48Mhz so 48MHz / 32 = 1,500 kHz ADC clock.
    .gclk = GCLK_CLKCTRL_GEN_GCLK0,
    .gclk_prescaler = ADC_CTRLB_PRESCALER_DIV32,
    // Max impedance is 17,617 kOhms
    .sample_time = 1,
    // Record and average 16 samples for each measurement.
    .sample_num = ADC_AVGCTRL_SAMPLENUM_16,
    .adjres = ADC_AVGCTRL_ADJRES(4),
};

/* Dotstar/animation constants */

#define GEM_ANIMATION_INTERVAL 48

/* Hard sync button configuration. */

static const struct WntrGPIOPin button_pin_ = WNTR_GPIO_PIN(B, 8);

/* Behavioral constants. */

#define GEM_CV_DEFAULT_BASE_OFFSET F16(1.0)
#define GEM_TWEAK_MAX_LFO_FREQ F16(6)
#define GEM_PULSE_WIDTH_MAX (3100)
#define GEM_PULSE_WIDTH_MOD_MAX (1920)
#define GEM_FM_DEADZONE F16(0.06)
