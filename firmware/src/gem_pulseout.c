#include "gem_pulseout.h"
#include "gem_config.h"

/* Public functions */

void gem_pulseout_init() {
    /* Enable the APB clock for TCC0 & TCC1. */
    PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1 | PM_APBCMASK_TCC2;

    /* Enable GCLK1 and wire it up to TCC0, TCC2, and TCC2. */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GEM_PULSEOUT_GCLK | GCLK_CLKCTRL_ID_TCC0_TCC1;
    while (GCLK->STATUS.bit.SYNCBUSY) {};
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GEM_PULSEOUT_GCLK | GCLK_CLKCTRL_ID_TCC2_TC3;
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Configure the clock prescaler for each TCC.
        This lets you divide up the clocks frequency to make the TCC count slower
        than the clock. In this case, I'm dividing the 8MHz clock by 16 making the
        TCC operate at 500kHz. This means each count (or "tick") is 2us.
    */
    TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER(GEM_PULSEOUT_GCLK_DIV);
    TCC2->CTRLA.reg |= TCC_CTRLA_PRESCALER(GEM_PULSEOUT_GCLK_DIV);

    /* Use "Normal PWM" */
    TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
    while (TCC0->SYNCBUSY.bit.WAVE) {};
    TCC2->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
    while (TCC2->SYNCBUSY.bit.WAVE) {};

    /* Configure pins. */
    PORT->Group[GEM_TCC0_PIN_PORT].DIRSET.reg = (1 << GEM_TCC0_PIN);
    PORT->Group[GEM_TCC0_PIN_PORT].OUTCLR.reg = (1 << GEM_TCC0_PIN);
    PORT->Group[GEM_TCC0_PIN_PORT].PINCFG[GEM_TCC0_PIN].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[GEM_TCC0_PIN_PORT].PMUX[GEM_TCC0_PIN >> 1].reg |= GEM_TCC0_PIN_FUNC;
    PORT->Group[GEM_TCC2_PIN_PORT].DIRSET.reg = (1 << GEM_TCC2_PIN);
    PORT->Group[GEM_TCC2_PIN_PORT].OUTCLR.reg = (1 << GEM_TCC2_PIN);
    PORT->Group[GEM_TCC2_PIN_PORT].PINCFG[GEM_TCC2_PIN].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[GEM_TCC2_PIN_PORT].PMUX[GEM_TCC2_PIN >> 1].reg |= GEM_TCC2_PIN_FUNC;

    /* Enable output */
    TCC0->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    while (TCC0->SYNCBUSY.bit.ENABLE) {};
    TCC2->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    while (TCC2->SYNCBUSY.bit.ENABLE) {};
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
            TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
            while (TCC0->SYNCBUSY.bit.COUNT) {};
            if (TCC0->COUNT.reg >= period) {
                TCC0->COUNT.reg = TCC0->COUNT.reg % period;
            }
            TCC0->PER.reg = period;
            break;

        case 1:
            TCC2->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
            while (TCC2->SYNCBUSY.bit.COUNT) {};
            if (TCC2->COUNT.reg >= period) {
                TCC2->COUNT.reg = TCC2->COUNT.reg % period;
            }
            TCC2->PER.reg = period;
            break;

        default:
            break;
    }
}

void gem_pulseout_set_duty(uint8_t channel, float duty) {
    switch (channel) {
        case 0:
            TCC0->CC[GEM_TCC0_WO].reg = (uint32_t)(TCC0->PER.reg * duty);
            break;

        case 1:
            TCC2->CC[GEM_TCC2_WO].reg = (uint32_t)(TCC2->PER.reg * duty);
            break;

        default:
            break;
    }
}

void gem_pulseout_phase_offset(float offset) {
    TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
    while (TCC0->SYNCBUSY.bit.COUNT) {};

    TCC1->COUNT.reg = (TCC0->COUNT.reg + (uint32_t)(TCC2->PER.reg * offset)) % TCC2->PER.reg;
}