/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_timer.h"
#include "gem_config.h"
#include "wntr_ramfunc.h"

static gem_timer_callback timer_callback;

/* Public functions */

void gem_timer_init(gem_timer_callback callback) {
    timer_callback = callback;

    /* Enable the APB clock for TC3 */
    PM->APBCMASK.reg |= PM_APBCMASK_TC3;

    /* Enable GCLK1 and wire it up to TC3 */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GEM_TC_TIMER_GCLK | GCLK_CLKCTRL_ID_TCC2_TC3;
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Reset the TC. */
    TC3->COUNT8.CTRLA.bit.ENABLE = 0;
    while (TC3->COUNT8.STATUS.bit.SYNCBUSY) {};
    TC3->COUNT8.CTRLA.bit.SWRST = 1;
    while (TC3->COUNT8.STATUS.bit.SYNCBUSY || TC3->COUNT8.CTRLA.bit.SWRST) {};

    /* 8-bit counter, normal frequency generation */
    TC3->COUNT8.CTRLA.reg |= (TC_CTRLA_MODE_COUNT8 | TC_CTRLA_WAVEGEN_NFRQ | GEM_TC_TIMER_GCLK_DIV);
    TC3->COUNT8.PER.reg = 1;

    /* Enable the TC */
    TC3->COUNT8.CTRLA.reg |= TCC_CTRLA_ENABLE;
    while (TC3->COUNT8.STATUS.bit.SYNCBUSY) {};

    /* Enable the overflow interrupt. */
    TC3->COUNT8.INTENSET.bit.OVF = 1;
    NVIC_SetPriority(TC3_IRQn, 0);
    NVIC_EnableIRQ(TC3_IRQn);
}

void TC3_Handler(void) RAMFUNC;

void TC3_Handler(void) {
    __disable_irq();
    TC3->COUNT8.INTFLAG.reg = TC_INTFLAG_OVF;
    if (timer_callback != NULL) {
        timer_callback();
    }
    __enable_irq();
}
