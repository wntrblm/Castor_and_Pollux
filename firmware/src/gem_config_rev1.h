/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "gem_adc.h"
#include "gem_dotstar.h"
#include "gem_i2c.h"
#include "gem_oscillator.h"
#include "gem_pulseout.h"
#include "gem_spi.h"

/*
    With the below settings the total time per ADC conversion is
    35.33 microseconds.

    With Gemini's 9 channels it takes at least 318 microseconds to measure all
    channels. There's a bit of added overhead because the CPU has to switch
    channels between each measurement. This means that the effective incoming
    sample rate for Gemini is about 3 kHz.

    See: https://blog.thea.codes/getting-the-most-out-of-the-samd21-adc/
*/
static const struct GemADCConfig GEM_REV1_ADC_CFG = {
    // ADC clock can be *at most* 2,100 kHz.
    // Gemini has GCLK0 @ 48Mhz so 48MHz / 32 = 1,500 kHz ADC clock.
    .gclk = GCLK_CLKCTRL_GEN_GCLK0,
    .gclk_prescaler = ADC_CTRLB_PRESCALER_DIV32,
    // Max impedance is 17,617 kOhms
    .sample_time = 1,
    // Record and average 32 samples for each measurement.
    .sample_num = ADC_AVGCTRL_SAMPLENUM_32,
    .adjres = ADC_AVGCTRL_ADJRES(4),
};

static const struct GemADCInput GEM_REV1_ADC_INPUTS[] = {
    GEM_ADC_INPUT(A, 5, 5),            // Duty A
    GEM_ADC_INPUT_INVERTED(A, 8, 16),  // Duty A Pot
    GEM_ADC_INPUT(B, 9, 3),            // Duty B
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
static const struct GemOscillatorInputConfig GEM_REV1_OSC_INPUT_CFG = {
    .pitch_cv_min = F16(0.0), .pitch_cv_max = F16(6.0)};

/*
    Configuration for the square wave outputs for each oscillator.

    Rev 1-4:
    TCC0 WO7 / PA17 for Castor
    TCC1 WO1 / PA11 for Pollux
*/
static const struct GemPulseOutConfig GEM_REV1_PULSE_OUT_CFG = {
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
static const struct GemI2CConfig GEM_REV1_I2C_CFG = {
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
static const struct GemSPIConfig GEM_REV1_SPI_CFG = {
    .gclk = GCLK_CLKCTRL_GEN_GCLK0,
    .gclk_freq = 48000000,
    .baud = 8000000,
    .polarity = 0,
    .phase = 0,
    .sercom = SERCOM1,
    .apbcmask = PM_APBCMASK_SERCOM1,
    .clkctrl_id = GCLK_CLKCTRL_ID_SERCOM1_CORE,
    .dopo = 0x0, /* Pad 0 is data, pad 1 is clock */
    .sck_pin = WNTR_GPIO_PIN_ALT(A, 0, D),
    .sdo_pin = WNTR_GPIO_PIN_ALT(B, 22, D),
};

static const struct GemDotstarCfg GEM_REV1_DOTSTAR_CFG = {
    .count = 7,
    .spi = &GEM_REV1_SPI_CFG,
};
