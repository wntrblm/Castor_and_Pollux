/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_pulseout.h"
#include "gem_config.h"
#include "wntr_ramfunc.h"

/* Forward declarations */
static void setup_tcc_(Tcc* tcc, size_t wo, const struct WntrGPIOPin pin);

/* Static globals */

static gem_pulseout_ovf_callback ovf_callback_;

/* Public functions */

void gem_pulseout_init(const struct GemPulseOutConfig* po, gem_pulseout_ovf_callback ovf_callback) {
    /* Enable the APB clock for TCC0 & TCC1. */
    PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1;

    /* Enable GCLK1 and wire it up to TCC0 & TCC1 */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | po->gclk | GCLK_CLKCTRL_ID_TCC0_TCC1;
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    setup_tcc_(TCC0, po->tcc0_wo, po->tcc0_pin);
    setup_tcc_(TCC1, po->tcc1_wo, po->tcc1_pin);

    /* Enable interrupts */
    ovf_callback_ = ovf_callback;
    TCC0->INTENSET.bit.OVF = 1;
    NVIC_SetPriority(TCC0_IRQn, 0);
    NVIC_EnableIRQ(TCC0_IRQn);
}

void gem_pulseout_set_period(const struct GemPulseOutConfig* po, uint8_t channel, uint32_t period) {
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
            TCC0->CCB[po->tcc0_wo % 4].reg = (uint32_t)(period / 2);
            break;

        case 1:
            TCC1->PER.bit.PER = period;
            TCC1->CCB[po->tcc1_wo % 4].reg = (uint32_t)(period / 2);
            break;

        default:
            break;
    }
}

/* Interrupt handlers */

void RAMFUNC TCC0_Handler(void) {
    TCC0->INTFLAG.reg = TCC_INTFLAG_MASK;
    if (ovf_callback_) {
        ovf_callback_(0);
    }
}

/* Private functions */

static void setup_tcc_(Tcc* tcc, size_t wo, const struct WntrGPIOPin pin) {
    /* Reset */
    tcc->CTRLA.bit.ENABLE = 0;
    while (tcc->SYNCBUSY.bit.ENABLE) {};
    tcc->CTRLA.bit.SWRST = 1;
    while (tcc->SYNCBUSY.bit.SWRST || tcc->CTRLA.bit.SWRST) {};

    /* Configure the TCC
    - No clock division.
    - Change the direction to count downwards, which makes it easier to change the PER register
        without worrying about the counter counting past the TOP value. See Datasheet Figure 31-10.
    */
    tcc->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;
    tcc->CTRLBSET.bit.DIR = 1;

    /* Configure the waveform output.
    - Use "Normal PWM", which generates a full cycle of a square wave every period.
    - Invert the polarity of the square wave outputs.
    */
    tcc->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM | TCC_WAVE_POL0 | TCC_WAVE_POL1 | TCC_WAVE_POL2 | TCC_WAVE_POL3;
    while (tcc->SYNCBUSY.bit.WAVE) {};

    /* Give the period and compare registers initial values */
    tcc->PER.bit.PER = 100;
    tcc->CC[wo % 4].reg = 1;

    /* Configure pins. */
    WntrGPIOPin_set_as_output(pin);
    WntrGPIOPin_configure_alt(pin);

    /* Enable the timer */
    tcc->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    while (tcc->SYNCBUSY.bit.ENABLE) {};
}
