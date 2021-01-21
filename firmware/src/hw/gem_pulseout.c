/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_pulseout.h"
#include "gem_config.h"

static uint32_t _timer_2_period = 0;
static bool _hard_sync = false;

/* Public functions */

void gem_pulseout_init() {
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
        than the clock. In this case, I'm dividing the 8MHz clock by 16 making the
        TCC operate at 500kHz. This means each count (or "tick") is 2us.
    */
    TCC0->CTRLA.reg |= GEM_PULSEOUT_GCLK_DIV;
    TCC1->CTRLA.reg |= GEM_PULSEOUT_GCLK_DIV;

    /* Use "Normal PWM" */
    TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
    while (TCC0->SYNCBUSY.bit.WAVE) {};
    TCC1->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
    while (TCC1->SYNCBUSY.bit.WAVE) {};

    /* We have to set some sort of period to begin with, otherwise the
        double-buffered writes won't work. */
    TCC0->PER.reg = 100;
    TCC1->PER.reg = 100;

    /* Configure pins. */
    PORT->Group[GEM_TCC0_PIN_PORT].DIRSET.reg = (1 << GEM_TCC0_PIN);
    PORT->Group[GEM_TCC0_PIN_PORT].OUTCLR.reg = (1 << GEM_TCC0_PIN);
    PORT->Group[GEM_TCC0_PIN_PORT].PINCFG[GEM_TCC0_PIN].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[GEM_TCC0_PIN_PORT].PMUX[GEM_TCC0_PIN >> 1].reg |= GEM_TCC0_PIN_FUNC;
    PORT->Group[GEM_TCC1_PIN_PORT].DIRSET.reg = (1 << GEM_TCC1_PIN);
    PORT->Group[GEM_TCC1_PIN_PORT].OUTCLR.reg = (1 << GEM_TCC1_PIN);
    PORT->Group[GEM_TCC1_PIN_PORT].PINCFG[GEM_TCC1_PIN].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[GEM_TCC1_PIN_PORT].PMUX[GEM_TCC1_PIN >> 1].reg |= GEM_TCC1_PIN_FUNC;

    /* Enable output */
    TCC0->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    while (TCC0->SYNCBUSY.bit.ENABLE) {};
    TCC1->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    while (TCC1->SYNCBUSY.bit.ENABLE) {};

    /* Enable interrupt. */
    TCC0->INTENSET.bit.OVF = 1;
    NVIC_SetPriority(TCC0_IRQn, 1);
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
            TCC0->PERB.bit.PERB = period;
            TCC0->CCB[GEM_TCC0_WO].reg = (uint32_t)(period / 2);
            break;

        case 1:
            TCC1->PERB.bit.PERB = period;
            TCC1->CCB[GEM_TCC1_WO].reg = (uint32_t)(period / 2);
            _timer_2_period = period;
            break;

        default:
            break;
    }
}

void gem_pulseout_hard_sync(bool state) { _hard_sync = state; }

void TCC0_Handler(void) {
    TCC0->INTFLAG.reg = TCC_INTFLAG_OVF;

    if (_hard_sync) {
        TCC1->CTRLBSET.reg = TCC_CTRLBSET_CMD_RETRIGGER;
    }
}
