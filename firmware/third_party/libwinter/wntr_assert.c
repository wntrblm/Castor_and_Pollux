/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_assert.h"
#include "wntr_mtb.h"

#ifdef __arm__
#include "printf.h"
#include "sam.h"
#else
#include <assert.h>
#include <stdio.h>
#endif

void _wntr_assert(const char* file, int line) {
#ifdef __arm__
    /*
        Disable the micro trace buffer so that the following operations don't
        pollute the MTB.
    */
    wntr_mtb_disable();

    /* Nothing should interrupt this. */
    __disable_irq();
#endif

    /*
        Print out the file & line using printf. This can be viewed using
        the JLinkRTT viewer.
    */
    printf("Assertion failed @ %s:%d.", file, line);

#ifdef __arm__
    /*
        Is a debugger present? if so, break. This register is specific to
        the SAMD21
    */
    if (DSU->STATUSB.bit.DBGPRES == 1) {
        __BKPT(0);
    }

    /* Failed assertions are never recoverable. Reset the system. */
    NVIC_SystemReset();
#else
    assert(0);
#endif
}
