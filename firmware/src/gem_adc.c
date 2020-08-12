#include "gem_adc.h"
#include "gem_config.h"
#include "sam.h"


static volatile uint32_t result;


void gem_adc_init() {
    /* Enable the APB clock for the ADC. */
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;

    /* Enable GCLK1 for the ADC */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
                        GEM_ADC_GCLK |
                        GCLK_CLKCTRL_ID_ADC;

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    uint32_t bias = (*((uint32_t *) ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
    uint32_t linearity = (*((uint32_t *) ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
    linearity |= ((*((uint32_t *) ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;

    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    /* Write the calibration data. */
    ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);

    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    ADC->CTRLB.reg |= GEM_ADC_PRESCALER;

    ADC->AVGCTRL.reg |= GEM_ADC_SAMPLE_NUM | GEM_ADC_SAMPLE_ADJRES;

    #if GEM_ADC_SAMPLE_NUM == ADC_AVGCTRL_SAMPLENUM_1
        ADC->CTRLB.reg |= ADC_CTRLB_RESSEL_12BIT;
    #else
        // 16-bit result needed for multisampling
        ADC->CTRLB.reg |= ADC_CTRLB_RESSEL_16BIT;
    #endif

    /* Configure the measurement parameters. */


    #ifndef GEM_ADC_USE_EXTERNAL_REF
        /*
        - Use the internal VCC reference. This is 1/2 of what's on VCCA.
            since VCCA is typically 3.3v, this is 1.65v.
        - GAIN_DIV2 means that the input voltage is halved. This is important
            because the voltage reference is 1/2 of VCCA. So if you want to
            measure 0-3.3v, you need to halve the input as well.
        */
        ADC->REFCTRL.reg |= ADC_REFCTRL_REFSEL_INTVCC1;
        ADC->INPUTCTRL.reg |= ADC_INPUTCTRL_GAIN_DIV2 | ADC_INPUTCTRL_MUXNEG_GND;
    #else
        #error External ref code not written yet
    #endif


    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    /* Enable the ADC. */
    ADC->CTRLA.bit.ENABLE = true;

    /*
    - MUXPOS_PIN3 means that the ADC should read from AIN3, or PB09.
        This is A2 on the Feather M0 board.
    */
    ADC->INPUTCTRL.reg |= ADC_INPUTCTRL_MUXPOS_PIN3;

    /* Set PB09 as an input pin. */
    PORT->Group[1].DIRCLR.reg = PORT_PB09;

    /* Enable the peripheral multiplexer for PB09. */
    PORT->Group[1].PINCFG[9].reg |= PORT_PINCFG_PMUXEN;

    /* Set PB09 to function B which is analog input. */
    PORT->Group[1].PMUX[4].reg = PORT_PMUX_PMUXO_B;


    /* Make one read and throw it away, as per the datasheet */
    gem_adc_read_sync();
}


uint16_t gem_adc_read_sync() {
    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {};

    /* Start the ADC using a software trigger. */
    ADC->SWTRIG.bit.START = true;

    /* Wait for the result ready flag to be set. */
    while (ADC->INTFLAG.bit.RESRDY == 0);

    /* Clear the flag. */
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;

    /* Read the value. */
    return ADC->RESULT.reg;
}