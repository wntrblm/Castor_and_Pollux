/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_assert.h"
#include "printf.h"
#include "sam.h"
#include "wntr_mtb.h"

void _wntr_assert(const char* file, int line) {
    /*
        Disable the micro trace buffer so that the following operations don't
        pollute the MTB.
    */
    wntr_mtb_disable();

    /* Nothing should interrupt this. */
    __disable_irq();

    /*
        Print out the file & line using printf. This can be viewed using
        the JLinkRTT viewer.
    */
    printf("Assertion failed @ %s:%d.", file, line);

    /*
        Is a debugger present? if so, break. This register is specific to
        the SAMD21
    */
    if (DSU->STATUSB.bit.DBGPRES == 1) {
        __BKPT(0);
    }

    /* Failed assertions are never recoverable. Reset the system. */
    NVIC_SystemReset();
}
