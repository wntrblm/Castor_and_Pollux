/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Global configuration for all of Gemini's hardware and behavior. */

#include "gem_adc.h"
#include "sam.h"
#include "wntr_gpio.h"
#include <stdint.h>

/* Analog to Digital Converter Configuration */

/*
    ADC clock can be *at most* 2,100 kHz.
    Gemini has GCLK0 @ 48Mhz so 48MHz / 32 = 1,500 kHz ADC clock.
*/
#define GEM_ADC_GCLK GCLK_CLKCTRL_GEN_GCLK0
#define GEM_ADC_PRESCALER ADC_CTRLB_PRESCALER_DIV32

/* Max impedance is 59,851 kOhms */
#define GEM_ADC_SAMPLE_TIME 5

/* Record and average 32 samples for each measurement. */
#define GEM_ADC_SAMPLE_NUM ADC_AVGCTRL_SAMPLENUM_32
#define GEM_ADC_SAMPLE_ADJRES ADC_AVGCTRL_ADJRES(4)

/*
    With the above settings the total time per ADC conversion is
    78 microseconds.

    With Gemini's 9 channels it takes at least 702 microseconds to measure all
    channels. There's a bit of added overhead because the CPU has to switch
    channels between each measurement. This means that the effective sample
    rate for Gemini is about 1.4 kHz.

    See: https://blog.thea.codes/getting-the-most-out-of-the-samd21-adc/
*/

/*
    A list of physical input pins for the ADC to measure during each
    scan.
*/
static const struct GemADCInput gem_adc_inputs[] = {
    {0, 6, ADC_INPUTCTRL_MUXPOS_PIN6},   // CV A
    {1, 3, ADC_INPUTCTRL_MUXPOS_PIN11},  // CV A Pot
    {0, 7, ADC_INPUTCTRL_MUXPOS_PIN7},   // CV B
    {1, 2, ADC_INPUTCTRL_MUXPOS_PIN10},  // CV B Pot
    {0, 5, ADC_INPUTCTRL_MUXPOS_PIN5},   // Duty A
    {0, 8, ADC_INPUTCTRL_MUXPOS_PIN16},  // Duty A Pot
    {1, 9, ADC_INPUTCTRL_MUXPOS_PIN3},   // Duty B
    {0, 9, ADC_INPUTCTRL_MUXPOS_PIN17},  // Duty B Pot
    {0, 2, ADC_INPUTCTRL_MUXPOS_PIN0},   // Chorus pot
};

/* Gives useful names to the inputs above. */
enum GemADCChannel {
    GEM_IN_CV_A,
    GEM_IN_CV_A_POT,
    GEM_IN_CV_B,
    GEM_IN_CV_B_POT,
    GEM_IN_DUTY_A,
    GEM_IN_DUTY_A_POT,
    GEM_IN_DUTY_B,
    GEM_IN_DUTY_B_POT,
    GEM_IN_CHORUS_POT,
    GEM_IN_COUNT,
};

/* Pulse out/TCC configuration. */

#define GEM_PULSEOUT_GCLK GCLK_CLKCTRL_GEN_GCLK1
#define GEM_PULSEOUT_GCLK_DIV TCC_CTRLA_PRESCALER_DIV1

/*
    TCC0 WO7 / PA17 for Castor
    TCC2 WO0 / PA16 for Pollux (rev 0)
    TCC1 WO1 / PA11 for Pollux (rev 1)
*/

#define GEM_TCC0_PIN_PORT 0
#define GEM_TCC0_PIN 17
#define GEM_TCC0_PIN_FUNC PORT_PMUX_PMUXO_F
#define GEM_TCC0_WO 3

#define GEM_TCC1_PIN_PORT 0
#define GEM_TCC1_PIN 11
#define GEM_TCC1_PIN_FUNC PORT_PMUX_PMUXO_E
#define GEM_TCC1_WO 1

/* I2C configuration for the DAC. */

#define GEM_I2C_GCLK GCLK_CLKCTRL_GEN_GCLK1
/* Hz */
#define GEM_I2C_GCLK_FREQ 8000000
/* Hz */
#define GEM_I2C_BAUDRATE 400000
/* Nanoseconds */
#define GEM_I2C_RISE_TIME 300
#define GEM_I2C_SERCOM SERCOM1
#define GEM_I2C_SERCOM_APBCMASK PM_APBCMASK_SERCOM1
#define GEM_I2C_GCLK_CLKCTRL_ID GCLK_CLKCTRL_ID_SERCOM1_CORE
#define GEM_I2C_PAD_0_PORT 0
#define GEM_I2C_PAD_0_PIN 0
#define GEM_I2C_PAD_0_PIN_FUNC WNTR_PMUX_D
#define GEM_I2C_PAD_1_PORT 0
#define GEM_I2C_PAD_1_PIN 1
#define GEM_I2C_PAD_1_PIN_FUNC WNTR_PMUX_D
#define GEM_I2C_WAIT_TIMEOUT 100000

/* SPI configuration for the Dotstars. */

#define GEM_SPI_GCLK GCLK_CLKCTRL_GEN_GCLK0
#define GEM_SPI_GCLK_FREQ 48000000
#define GEM_SPI_SERCOM SERCOM5
#define GEM_SPI_SERCOM_APBCMASK PM_APBCMASK_SERCOM5
#define GEM_SPI_GCLK_CLKCTRL_ID GCLK_CLKCTRL_ID_SERCOM5_CORE
#define GEM_SPI_BAUD 8000000
#define GEM_SPI_POLARITY 0
#define GEM_SPI_PHASE 0
/* Pad 2 is clock, pad 3 is data */
#define GEM_SPI_DOPO 0x1
#define GEM_SPI_SCK_PORT 1
#define GEM_SPI_SCK_PIN 23
#define GEM_SPI_SCK_PIN_FUNC WNTR_PMUX_D
#define GEM_SPI_SDO_PORT 1
#define GEM_SPI_SDO_PIN 22
#define GEM_SPI_SDO_PIN_FUNC WNTR_PMUX_D

/* Dotstar configuration */

#define GEM_DOTSTAR_COUNT 7
#define GEM_ANIMATION_INTERVAL 50

/* Hard sync button configuration. */

#define GEM_HARD_SYNC_BUTTON_PORT 1
#define GEM_HARD_SYNC_BUTTON_PIN 8

/* Behavioral configuration. */

#define GEM_CV_INPUT_RANGE F16(6.0)
#define GEM_CV_BASE_OFFSET F16(1.0)
#define GEM_TWEAK_MAX_LFO_FREQUENCY F16(5)

/* The size of the look-up table used for making the tuning knobs non-linear. */
#define GEM_KNOB_BEZIER_LUT_LEN 256
