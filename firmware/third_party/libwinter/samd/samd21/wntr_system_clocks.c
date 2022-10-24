/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_system_clocks.h"
#include "sam.h"
#include "wntr_fuses.h"

void wntr_system_clocks_init() {
    /* Switch CPU to 8Mhz for now by disabling the prescaler */
    SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
    SYSCTRL->OSC8M.bit.PRESC = 0;

    /* Configure the DFLL to generate a 48MHz clock. */

    /* Configure the DFLL in open loop mode / USB CRM. */

    /* This is needed as per the errata - accessing the DPLL before doing this can lock the processor. */
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    /* Write the coarse and fine calibration from NVM. */
    uint32_t coarse_cal = OTP4_FUSES->FUSES0.bit.DFLL48M_COARSE_CAL;
    uint32_t fine_cal = OTP4_FUSES->FUSES1.bit.DFLL48M_FINE_CAL;

    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse_cal) | SYSCTRL_DFLLVAL_FINE(fine_cal);
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    /* Enable USB clock recovery mode. */
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_CCDIS;

    /*
        From datasheet section 37.15, Note 1: When using DFLL48M in USB recovery mode,
        the Fine Step value must be Ah to guarantee a USB clock at
        +/-0.25% before 11ms after a resume.
    */
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_MUL(48000) | SYSCTRL_DFLLMUL_CSTEP(1) | SYSCTRL_DFLLMUL_FSTEP(0xA);

    /* Setting to closed loop mode with USBCRM means that
       the DFLL will operate in closed loop when USB is
       connected, but will otherwise operate in open
       loop mode. */
    SYSCTRL->DFLLCTRL.bit.MODE = 1;

    /* Enable the DFLL. */
    SYSCTRL->DFLLCTRL.bit.ENABLE = 1;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    /* Before switching the CPU clock, change the number of NVM wait states. */
    /* 1 wait state required @ 3.3V & 48MHz according to table 37-40 in data sheet. */
    NVMCTRL->CTRLB.bit.RWS = 1;

    /* Switch GCLK0, and therefore the CPU, to the DFLL. */
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M;

    /* GENCTRL is Write-Synchronized...so wait for write to complete */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* CPU clock is now 48 MHz */
    SystemCoreClock = 48000000;

    /* Configure GCLK1 to follow the 8MHz oscillator.
        This is used to clock the ADC & TCC peripherals.
    */

    /* Disable division */
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(1);

    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(1) | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_GENEN;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {};
}
