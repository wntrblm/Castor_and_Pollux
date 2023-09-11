/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "gem_adc.h"
#include "gem_dotstar.h"
#include "gem_i2c.h"
#include "gem_led_animation.h"
#include "gem_oscillator.h"
#include "gem_pulseout.h"
#include "gem_spi.h"

static const struct GemADCInput GEM_I_ADC_INPUTS[] = {
    GEM_ADC_INPUT_INVERTED(A, 5, 5),   // Duty A
    GEM_ADC_INPUT_INVERTED(A, 8, 16),  // Duty A Pot
    GEM_ADC_INPUT_INVERTED(B, 9, 3),   // Duty B
    GEM_ADC_INPUT_INVERTED(A, 9, 17),  // Duty B Pot
    GEM_ADC_INPUT_INVERTED(A, 2, 0),   // Chorus pot
    GEM_ADC_INPUT_INVERTED(B, 3, 11),  // CV A Pot
    GEM_ADC_INPUT_INVERTED(B, 2, 10),  // CV B Pot
    GEM_ADC_INPUT(A, 6, 6),            // CV A
    GEM_ADC_INPUT(A, 7, 7),            // CV B
};

/*
    Configuration for the oscillator's input range
*/
static const struct GemOscillatorInputConfig GEM_I_OSC_INPUT_CFG = {
    .pitch_cv_min = F16(0.0),
    .pitch_cv_max = F16(6.0),
};

/*
    Configuration for the square wave outputs for each oscillator.

    Rev 1-4:
    TCC0 WO7 / PA17 for Castor
    TCC1 WO1 / PA11 for Pollux
*/
static const struct GemPulseOutConfig GEM_I_PULSE_OUT_CFG = {
    .gclk = GCLK_CLKCTRL_GEN_GCLK1,
    .gclk_freq = 8000000,
    .tcc0_pin = WNTR_GPIO_PIN_ALT(A, 17, F),
    .tcc0_wo = 7,
    .tcc1_pin = WNTR_GPIO_PIN_ALT(A, 11, E),
    .tcc1_wo = 1,
};

/*
    DAC I2C configuration
*/
static const struct GemI2CConfig GEM_I_I2C_CFG = {
    .gclk = GCLK_CLKCTRL_GEN_GCLK1,
    .gclk_freq = 8000000,
    .baudrate = 400000,
    .rise_time = 30,
    .wait_timeout = 10000,
    .sercom = SERCOM1,
    .apbcmask = PM_APBCMASK_SERCOM1,
    .clkctrl_id = GCLK_CLKCTRL_ID_SERCOM1_CORE,
    .pad0_pin = WNTR_GPIO_PIN_ALT(A, 0, D),
    .pad1_pin = WNTR_GPIO_PIN_ALT(A, 1, D),
};

/*
    Dotstar & SPI configuration
*/
static const struct GemSPIConfig GEM_I_SPI_CFG = {
    .gclk = GCLK_CLKCTRL_GEN_GCLK0,
    .gclk_freq = 48000000,
    .baud = 8000000,
    .polarity = 0,
    .phase = 0,
    .sercom = SERCOM5,
    .apbcmask = PM_APBCMASK_SERCOM5,
    .clkctrl_id = GCLK_CLKCTRL_ID_SERCOM5_CORE,
    .dopo = 0x1, /* Pad 3 is data, pad 2 is clock */
    .sck_pin = WNTR_GPIO_PIN_ALT(B, 23, D),
    .sdo_pin = WNTR_GPIO_PIN_ALT(B, 22, D),
};

static const struct GemDotstarCfg GEM_I_DOTSTAR_CFG = {
    .count = 7,
    .spi = &GEM_I_SPI_CFG,
};

/*
    Animation configuration
*/
static const struct GemLEDCfg GEM_I_LED_CFG = {
    .hue_offsets =
        {
            0,
            8169,
            16338,
            24508,
            32677,
            40846,
            49016,
            57185,
        },
    .vertical_pos_index = {4, 5, 3, 6, 0, 2, 1},
    .lfo_tweak_led = 2,
    .pitch_a_tweak_led = 1,
    .pitch_b_tweak_led = 3,
    .pwm_a_led = 6,
    .pwm_b_led = 4,
    .fm_a_led = 6,
    .fm_b_led = 4,
};
