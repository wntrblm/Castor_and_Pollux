/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_bod.h"
#include "sam.h"

void wntr_bod_wait_for_stable_voltage() {
    /*
        Configure the BOD to monitor for at least 3.0 volts and wait for
        VDD to reach that level.
    */
    SYSCTRL->BOD33.bit.ENABLE = 0;
    while (!SYSCTRL->PCLKSR.bit.B33SRDY) {};

    SYSCTRL->BOD33.reg = (
        /* VBOD-: 3.0, VBOD+: 3.3 See datasheet Table 37-21. */
        SYSCTRL_BOD33_LEVEL(48) |
        /* Don't reset */
        SYSCTRL_BOD33_ACTION_NONE |
        /* Enable hysteresis */
        SYSCTRL_BOD33_HYST);

    /* Wait for the voltage to stabilize. */
    SYSCTRL->BOD33.bit.ENABLE = 1;
    while (!SYSCTRL->PCLKSR.bit.BOD33RDY) {}
    while (SYSCTRL->PCLKSR.bit.BOD33DET) {}

    /*
        Everything is good, let the BOD33 reset the microcontroller if the
        voltage falls below 3.0 volts.
    */

    SYSCTRL->BOD33.bit.ENABLE = 0;
    while (!SYSCTRL->PCLKSR.bit.B33SRDY) {};
    SYSCTRL->BOD33.reg |= SYSCTRL_BOD33_ACTION_RESET;
    SYSCTRL->BOD33.bit.ENABLE = 1;
}
