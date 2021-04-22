#include "sam.h"

#ifdef SAMD21
#include "wntr_mtb.h"
#endif

/*
    Unfortunately, the Cortex M0+ doesn't provide a lot of information about hardfaults.
    So the best we can do it enter a breakpoint and hope the MTB contains enough
    information to ascertain the cause.
*/

void HardFault_Handler(void) {
    /* Nothing should interrupt this. */
    __disable_irq();

#ifdef SAMD21
    /* Disable the Microtrace Buffer so that it's state is frozen. */
    wntr_mtb_disable();
#endif

    /*
        Is a debugger present? if so, break. This register is specific to
        the SAMD21/SAMD51.
    */
    if (DSU->STATUSB.bit.DBGPRES == 1) {
        __BKPT(0);
    }

    /* Reset */
    NVIC_SystemReset();
}
