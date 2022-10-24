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
    SUPC->BOD33.bit.ENABLE = 0;
    while (!SUPC->STATUS.bit.B33SRDY) {}

    SUPC->BOD33.reg = (
        /* See Table 54-21. */
        SUPC_BOD33_LEVEL(0xFF) |
        /* Don't reset */
        SUPC_BOD33_ACTION_NONE);

    /* Wait for the voltage to stabilize above 3V. */
    SUPC->BOD33.bit.ENABLE = 1;
    while (!SUPC->STATUS.bit.BOD33RDY) {}
    while (SUPC->STATUS.bit.BOD33DET) {}

    /*
        Everything is good, let the BOD33 reset the microcontroller if the
        voltage falls below 3.0 volts.
    */

    SUPC->BOD33.bit.ENABLE = 0;
    while (!SUPC->STATUS.bit.B33SRDY) {};
    SUPC->BOD33.reg |= SUPC_BOD33_ACTION_RESET;
    SUPC->BOD33.bit.ENABLE = 1;
}
