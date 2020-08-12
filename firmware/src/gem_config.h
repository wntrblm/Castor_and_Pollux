#pragma once
#include "sam.h"

#define PIN_STATUS_LED PIN_PA18
#define PIN_STATUS_LED_PORT 0
#define PIN_STATUS_LED_2 PIN_PA07
#define PIN_STATUS_LED_2_PORT 0

#define PIN_BUTTON PIN_PA15
#define PIN_BUTTON_PORT 0


/* Analog to Digital Converter Configuration */

// 8MHz Clock / 4 = 2 kHz ADC clock
#define GEM_ADC_GCLK GCLK_CLKCTRL_GEN_GCLK0
#define GEM_ADC_PRESCALER ADC_CTRLB_PRESCALER_DIV4
#define GEM_ADC_REF internal

// Max impedance is 20kOhms
#define GEM_ADC_SAMPLE_TIME 2

#define GEM_ADC_SAMPLE_NUM ADC_AVGCTRL_SAMPLENUM_128
#define GEM_ADC_SAMPLE_ADJRES ADC_AVGCTRL_ADJRES(4)

// #define GEM_ADC_USE_EXTERNAL_REF