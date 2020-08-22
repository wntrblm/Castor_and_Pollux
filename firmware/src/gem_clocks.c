#include "gem_clocks.h"
#include "sam.h"

void gem_clocks_init() {
    /* Switch CPU to 8Mhz for now by disabling the prescaler */
    SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
    SYSCTRL->OSC8M.bit.PRESC = 0;

    /* Configure the DFLL to generate a 48MHz clock. */

    /* Configure the DFLL in open loop mode. */

    /* This is needed as per the errata - accessing the DPLL before doing this can lock the processor. */
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY)
        ;
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY)
        ;

    /* Write the coarse and fine calibration from NVM. */
    uint32_t coarse_cal =
        ((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
    uint32_t fine_cal =
        ((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR) & FUSES_DFLL48M_FINE_CAL_Msk) >> FUSES_DFLL48M_FINE_CAL_Pos;
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse_cal) | SYSCTRL_DFLLVAL_FINE(fine_cal);
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY)
        ;

    /* Enable USB clock recovery mode. */
    SYSCTRL->DFLLCTRL.bit.USBCRM = 1;
    SYSCTRL->DFLLCTRL.bit.CCDIS = 1;
    SYSCTRL->DFLLMUL.bit.MUL = 48000;
    SYSCTRL->DFLLMUL.bit.FSTEP = 1;
    SYSCTRL->DFLLMUL.bit.CSTEP = 1;

    /* Setting to closed loop mode with USBCRM means that
       the DFLL will operate in closed loop when USB is
       connected, but will otherwise operate in open
       loop mode. */
    SYSCTRL->DFLLCTRL.bit.MODE = 1;

    /* Enable the DFLL. */
    SYSCTRL->DFLLCTRL.bit.ENABLE = 1;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY)
        ;

    /* Before switching the CPU clock, change the number of NVM wait states. */
    /* 1 wait state required @ 3.3V & 48MHz according to table 37-40 in data sheet. */
    NVMCTRL->CTRLB.bit.RWS = 1;

    /* Switch GCLK0, and therefore the CPU, to the DFLL. */
    GCLK_GENCTRL_Type gclk0_genctrl = {
        .bit.RUNSTDBY = 0,                       /* Disable in standby */
        .bit.DIVSEL = 0,                         /* No division */
        .bit.OE = 0,                             /* No pin output */
        .bit.IDC = 1,                            /* Generator duty cycle is 50/50 */
        .bit.GENEN = 1,                          /* Enable the generator */
        .bit.SRC = GCLK_GENCTRL_SRC_DFLL48M_Val, /* Generator source: DFLL48M output */
        .bit.ID = 0                              /* Generator ID: 0 */
    };
    GCLK->GENCTRL.reg = gclk0_genctrl.reg;
    // GENCTRL is Write-Synchronized...so wait for write to complete
    while (GCLK->STATUS.bit.SYNCBUSY)
        ;

    /* Configure GCLK1 to follow the 8MHz oscillator.
        This is used to clock the ADC & TCC peripherals.
    */
    GCLK->GENDIV.bit.ID = 1;
    GCLK->GENDIV.bit.DIV = 0;

    GCLK->GENCTRL.bit.ID = 1;
    GCLK->GENCTRL.bit.SRC = GCLK_GENCTRL_SRC_OSC8M_Val;
    GCLK->GENCTRL.bit.GENEN = 1;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
        ;

    /* Configure GCLK2 to follow the 32kHz oscillator. */
    SYSCTRL->OSC32K.reg = SYSCTRL_OSC32K_ENABLE | SYSCTRL_OSC32K_EN32K;
    while (!SYSCTRL->PCLKSR.bit.OSC32KRDY)
        ;

    GCLK->GENDIV.bit.ID = 2;
    GCLK->GENDIV.bit.DIV = 0;

    GCLK->GENCTRL.bit.ID = 2;
    GCLK->GENCTRL.bit.SRC = GCLK_GENCTRL_SRC_OSC32K_Val;
    GCLK->GENCTRL.bit.GENEN = 1;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
        ;
}