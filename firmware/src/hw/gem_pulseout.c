/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_pulseout.h"
#include "gem_config.h"
#include "wntr_ramfunc.h"

static gem_pulseout_ovf_callback ovf_callback_;

/* Public functions */

void gem_pulseout_init(gem_pulseout_ovf_callback ovf_callback) {
    /* Enable the APB clock for TCC0 & TCC1. */
    PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1;

    /* Enable GCLK1 and wire it up to TCC0 & TCC1 */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GEM_PULSEOUT_GCLK | GCLK_CLKCTRL_ID_TCC0_TCC1;
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Reset TCCs. */
    TCC0->CTRLA.bit.ENABLE = 0;
    while (TCC0->SYNCBUSY.bit.ENABLE) {};
    TCC0->CTRLA.bit.SWRST = 1;
    while (TCC0->SYNCBUSY.bit.SWRST || TCC0->CTRLA.bit.SWRST) {};
    TCC1->CTRLA.bit.ENABLE = 0;
    while (TCC1->SYNCBUSY.bit.ENABLE) {};
    TCC1->CTRLA.bit.SWRST = 1;
    while (TCC1->SYNCBUSY.bit.SWRST || TCC1->CTRLA.bit.SWRST) {};

    /* Configure the clock prescaler for each TCC.
        This lets you divide up the clocks frequency to make the TCC count slower
        than the clock, in this case, I'm not dividing the clock at all.
    */
    TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;
    TCC1->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;

    /* Use downward counting, which makes it easier to change the PER register
       without worrying about the counter counting past the TOP value.

       See Datasheet Figure 31-10.
    */
    TCC0->CTRLBSET.bit.DIR = 1;
    TCC1->CTRLBSET.bit.DIR = 1;

    /* Use "Normal PWM" */
    TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM | TCC_WAVE_POL0 | TCC_WAVE_POL1 | TCC_WAVE_POL2 | TCC_WAVE_POL3;
    while (TCC0->SYNCBUSY.bit.WAVE) {};
    TCC1->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM | TCC_WAVE_POL0 | TCC_WAVE_POL1 | TCC_WAVE_POL2 | TCC_WAVE_POL3;
    while (TCC1->SYNCBUSY.bit.WAVE) {};

    TCC0->PER.bit.PER = 100;
    TCC1->PER.bit.PER = 100;
    TCC0->CC[GEM_TCC0_WO].reg = 1;
    TCC1->CC[GEM_TCC1_WO].reg = 1;

    /* Configure pins. */
    WntrGPIOPin_set_as_output(CASTOR_SQUARE_WAVE_PIN);
    WntrGPIOPin_configure_alt(CASTOR_SQUARE_WAVE_PIN, CASTOR_SQUARE_WAVE_PIN_ALT);
    WntrGPIOPin_set_as_output(POLLUX_SQUARE_WAVE_PIN);
    WntrGPIOPin_configure_alt(POLLUX_SQUARE_WAVE_PIN, POLLUX_SQUARE_WAVE_PIN_ALT);

    /* Enable the timers */
    TCC0->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    while (TCC0->SYNCBUSY.bit.ENABLE) {};
    TCC1->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    while (TCC1->SYNCBUSY.bit.ENABLE) {};

    /* Enable interrupts */
    ovf_callback_ = ovf_callback;
    TCC0->INTENSET.bit.OVF = 1;
    NVIC_SetPriority(TCC0_IRQn, 0);
    NVIC_EnableIRQ(TCC0_IRQn);
}

void gem_pulseout_set_period(uint8_t channel, uint32_t period) {
    /* Configure the frequency for the PWM by setting the PER register.
        The value of the PER register determines the frequency in the following
        way:

            frequency = GLCK frequency / (TCC prescaler * (1 + PER))

        For example if PER is 512 then frequency = 8Mhz / (16 * (1 + 512))
        so the frequency is 947Hz.
    */
    switch (channel) {
        case 0:
            TCC0->PER.bit.PER = period;
            TCC0->CCB[GEM_TCC0_WO].reg = period / 2;
            break;

        case 1:
            TCC1->PER.bit.PER = period;
            TCC1->CCB[GEM_TCC1_WO].reg = period / 2;
            break;

        default:
            break;
    }
}

void RAMFUNC TCC0_Handler(void) {
    TCC0->INTFLAG.reg = TCC_INTFLAG_MASK;
    if (ovf_callback_) {
        ovf_callback_(0);
    }
}
