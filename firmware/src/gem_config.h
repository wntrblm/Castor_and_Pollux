#pragma once
#include "gem_adc.h"
#include "sam.h"

/* This must be the same as the nvm region in the linker script. */
#define GEM_NVM_SETTINGS_BASE_ADDR (0x00040000 - 1024)

#define PIN_STATUS_LED PIN_PA18
#define PIN_STATUS_LED_PORT 0
#define PIN_STATUS_LED_2 PIN_PA07
#define PIN_STATUS_LED_2_PORT 0

#define PIN_BUTTON PIN_PA15
#define PIN_BUTTON_PORT 0

/* Analog to Digital Converter Configuration */

/*
    GCLK1: 8MHz Clock / DIV8 = 1 kHz ADC clock
    GCLK0: 48Mhz Clock / DIV32 = 1.5 kHz ADC clock
*/
#define GEM_ADC_GCLK GCLK_CLKCTRL_GEN_GCLK0
#define GEM_ADC_PRESCALER ADC_CTRLB_PRESCALER_DIV32
#define GEM_ADC_REF internal

/* Max impedance is 28kOhms */
#define GEM_ADC_SAMPLE_TIME 2

#define GEM_ADC_SAMPLE_NUM ADC_AVGCTRL_SAMPLENUM_256
#define GEM_ADC_SAMPLE_ADJRES ADC_AVGCTRL_ADJRES(4)

// #define GEM_ADC_USE_EXTERNAL_REF

extern const struct gem_adc_input gem_adc_inputs[];

/* Pulse out/TCC configuration. */

#define GEM_PULSEOUT_GCLK GCLK_CLKCTRL_GEN_GCLK1

/*
    TCC0 WO7 / PA17 for Castor
    TCC2 WO0 / PA16 for Pollux
*/

#define GEM_TCC0_PIN_PORT 0
#define GEM_TCC0_PIN PIN_PA17
#define GEM_TCC0_PIN_FUNC PORT_PMUX_PMUXO_F
#define GEM_TCC0_WO 3

#define GEM_TCC2_PIN_PORT 0
#define GEM_TCC2_PIN PIN_PA16
#define GEM_TCC2_PIN_FUNC PORT_PMUX_PMUXE_E
#define GEM_TCC2_WO 0

/* I2C configuration. */

#define GEM_I2C_GCLK GCLK_CLKCTRL_GEN_GCLK1
/* Hz */
#define GEM_I2C_GCLK_FREQ 8000000
/* Hz */
#define GEM_I2C_BAUDRATE 100000 // TODO: Change back to 400,000
/* Nanoseconds */
#define GEM_I2C_RISE_TIME 300
#define GEM_I2C_SERCOM SERCOM1
#define GEM_I2C_SERCOM_APBCMASK PM_APBCMASK_SERCOM1
#define GEM_I2C_GCLK_CLKCTRL_ID GCLK_CLKCTRL_ID_SERCOM1_CORE
#define GEM_I2C_PAD_0_PORT 0
#define GEM_I2C_PAD_0_PIN PIN_PA00
#define GEM_I2C_PAD_0_PIN_FUNC GEM_PMUX_D
#define GEM_I2C_PAD_1_PORT 0
#define GEM_I2C_PAD_1_PIN PIN_PA01
#define GEM_I2C_PAD_1_PIN_FUNC GEM_PMUX_D

/* Structs */

struct gem_nvm_settings {
    uint16_t adc_gain_corr;
    uint16_t adc_offset_corr;
    uint16_t led_brightness;
};

/* Functions */

void gem_config_init();
bool gem_config_get_nvm_settings(struct gem_nvm_settings* settings);
void gem_config_save_nvm_settings(struct gem_nvm_settings* settings);
void gem_config_erase_nvm_settings();

int __wrap_printf(const char* format, ...);