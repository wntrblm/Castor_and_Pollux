/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_adc.h"
#include "fix16.h"
#include "sam.h"
#include "wntr_assert.h"
#include "wntr_fuses.h"
#include "wntr_gpio.h"
#include "wntr_ramfunc.h"
#include "wntr_uint12.h"

/* Inputs to scan_next_channel. */
static const struct GemADCInput* inputs_;
static size_t num_inputs_;

/* Results from input scanning */
static volatile uint32_t* results_;

/* Scanning state */
static volatile size_t current_input_idx_ = 0;
static volatile bool results_ready_ = false;

/* Private forward declarations. */
static void scan_next_channel() RAMFUNC;

/* Public methods. */

void gem_adc_init(const struct GemADCConfig* adc, int16_t offset_error, uint16_t gain_error) {
    /* Enable the APB clock for the ADC. */
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;

    /* Enable GCLK1 for the ADC */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | adc->gclk | GCLK_CLKCTRL_ID_ADC;

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Reset the ADC. */
    ADC->CTRLA.bit.ENABLE = 0;
    while (ADC->STATUS.bit.SYNCBUSY) {};
    ADC->CTRLA.bit.SWRST = 1;
    while (ADC->CTRLA.bit.SWRST || ADC->STATUS.bit.SYNCBUSY) {};

    uint32_t bias = OTP4_FUSES->FUSES0.bit.ADC_BIASCAL;
    uint32_t linearity = OTP4_FUSES->FUSES0.bit.ADC_LINEARITY;

    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    /* Write the calibration data. */
    ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);

    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    ADC->CTRLB.reg |= adc->gclk_prescaler;

    ADC->AVGCTRL.reg |= adc->sample_num | adc->adjres;

    if (adc->sample_num == ADC_AVGCTRL_SAMPLENUM_1) {
        ADC->CTRLB.reg |= ADC_CTRLB_RESSEL_12BIT;
    } else {
        // 16-bit result needed for multisampling
        ADC->CTRLB.reg |= ADC_CTRLB_RESSEL_16BIT;
    }

    /* Configure the measurement parameters. */

    /*
    - Use the internal VCC reference. This is 1/2 of what's on VCCA.
        since VCCA is typically 3.3v, this is 1.65v.
    - GAIN_DIV2 means that the input voltage is halved. This is important
        because the voltage reference is 1/2 of VCCA. So if you want to
        measure 0-3.3v, you need to halve the input as well.
    */
    ADC->REFCTRL.reg |= ADC_REFCTRL_REFSEL_INTVCC1;
    ADC->INPUTCTRL.reg |= ADC_INPUTCTRL_GAIN_DIV2 | ADC_INPUTCTRL_MUXNEG_GND;

    /* Enable the reference buffer to increase accuracy (at the cost of speed). */
    ADC->REFCTRL.bit.REFCOMP = 1;

    /* Enable offset and error correction. */
    ADC->OFFSETCORR.reg = ADC_OFFSETCORR_OFFSETCORR(offset_error);
    ADC->GAINCORR.reg = ADC_GAINCORR_GAINCORR(gain_error);
    ADC->CTRLB.bit.CORREN = 1;

    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    /* Enable the ADC. */
    ADC->CTRLA.bit.ENABLE = 1;

    /* NOTE: The datasheet says to throw away the first reading, however,
        since Gemini does a *lot* of reads and uses averaging, this
        isn't really necessary.
    */
}

void gem_adc_set_error_correction(uint16_t gain, uint16_t offset) {
    ADC->CTRLA.bit.ENABLE = 0;
    while (ADC->STATUS.bit.SYNCBUSY) {};

    ADC->OFFSETCORR.reg = ADC_OFFSETCORR_OFFSETCORR(offset);
    ADC->GAINCORR.reg = ADC_GAINCORR_GAINCORR(gain);

    ADC->CTRLA.bit.ENABLE = 1;
    while (ADC->STATUS.bit.SYNCBUSY) {};
}

void gem_adc_init_input(const struct GemADCInput* input) {
    wntr_gpio_set_as_input(input->port, input->pin, false);
    wntr_gpio_configure_alt(input->port, input->pin, WNTR_PMUX_B);
}

uint16_t gem_adc_read_sync(const struct GemADCInput* input) {
    /* Stop channel scanning, we need exclusive control over the ADC. */
    gem_adc_stop_scanning();

    /* Flush the ADC - if there's a conversion in process it'll be cancelled. */
    ADC->SWTRIG.reg = ADC_SWTRIG_FLUSH;
    while (ADC->SWTRIG.bit.FLUSH) {};

    /* Set the positive mux to the input pin */
    ADC->INPUTCTRL.bit.MUXPOS = input->ain;
    while (ADC->STATUS.bit.SYNCBUSY) {};

    /* Start the ADC using a software trigger. */
    ADC->SWTRIG.bit.START = 1;

    /* Wait for the result ready flag to be set. */
    while (ADC->INTFLAG.bit.RESRDY == 0) {};

    /* Clear the flag. */
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;

    /*
        Throw away the first result and measure again - the datasheet
        recommends doing that since the first conversion for a new
        configuration will be incorrect.
    */
    ADC->SWTRIG.bit.START = 1;
    while (ADC->INTFLAG.bit.RESRDY == 0) {};
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;

    return ADC->RESULT.reg;
}

void gem_adc_start_scanning(const struct GemADCInput* inputs, size_t num_inputs, uint32_t* results) {
    inputs_ = inputs;
    num_inputs_ = num_inputs;
    current_input_idx_ = 0;
    results_ready_ = false;
    results_ = results;

    /* Enable ADC interrupts and the "result ready" interrupt */
    NVIC_SetPriority(ADC_IRQn, 1);
    NVIC_EnableIRQ(ADC_IRQn);
    ADC->INTENSET.bit.RESRDY = 1;

    /*
        Start scanning, the interrupt will keep calling scan_next_channel
        after each result is ready.
    */
    scan_next_channel();
}

void gem_adc_stop_scanning() { NVIC_DisableIRQ(ADC_IRQn); }
void gem_adc_resume_scanning() { NVIC_EnableIRQ(ADC_IRQn); }

bool gem_adc_results_ready() {
    if (results_ready_) {
        results_ready_ = false;
        return true;
    } else {
        return false;
    }
}

/* Private methods & interrupt handlers. */

static void scan_next_channel() {
    struct GemADCInput input = inputs_[current_input_idx_];

    /* Swap out the input pin. */
    ADC->INPUTCTRL.bit.MUXPOS = input.ain;

    /* Wait for synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    /* Start the ADC using a software trigger. */
    ADC->SWTRIG.bit.START = 1;
}

void ADC_Handler(void) RAMFUNC;

void ADC_Handler(void) {
    /*
        Should always be the result ready flag since its the only interrupt we
        use- if it isn't, something bad has happened. :(
    */
    if (!ADC->INTFLAG.bit.RESRDY) {
        WNTR_ASSERT(0);
        ADC->INTFLAG.reg = ADC_INTFLAG_RESETVALUE;
        return;
    }

    struct GemADCInput input = inputs_[current_input_idx_];

    /*
        Store the result, reading ADC->RESULT automatically clears the
        interrupt flag.
    */
    uint32_t result = ADC->RESULT.reg;
    if (input.invert) {
        result = UINT12_INVERT(result);
    }
    results_[current_input_idx_] = result;

    /* Scan the next input */
    current_input_idx_ = current_input_idx_ + 1;
    if (current_input_idx_ == num_inputs_) {
        current_input_idx_ = 0;
        results_ready_ = true;
    }

    scan_next_channel();
}
