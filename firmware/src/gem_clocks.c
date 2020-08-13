#include "sam.h"
#include "gem_clocks.h"


void gem_clocks_init() {
    // Switch CPU to 8Mhz by disabling the prescaler
    SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
    SYSCTRL->OSC8M.bit.PRESC = 0;

    /* Configure GCLK1 to follow the 8MHz oscillator.
        This is used to clock the ADC & TCC peripherals.
    */
    GCLK->GENDIV.bit.ID = 1;
    GCLK->GENDIV.bit.DIV = 0;

    GCLK->GENCTRL.bit.ID = 1;
    GCLK->GENCTRL.bit.SRC = GCLK_GENCTRL_SRC_OSC8M_Val;
    GCLK->GENCTRL.bit.GENEN = 1;
}