#include "samd21.h"
#include "wntr_bod.h"
#include "wntr_fuses.h"
#include "wntr_mtb.h"
#include "wntr_system_clocks.h"
#include "wntr_ticks.h"

/*
    Initial clock frequency on the SAMD21 is 1 MHz.
*/
#define STARTUP_SYSTEM_CLOCK (1000000)

uint32_t SystemCoreClock = STARTUP_SYSTEM_CLOCK;

void SystemInit(void) {
    SystemCoreClock = STARTUP_SYSTEM_CLOCK;

    /* Wait for brown-out detector */
    wntr_bod_wait_for_stable_voltage();

    /* Check bootloader flash protection */
    wntr_check_bootprot_fuse();

    /* Configure clocks */
    wntr_system_clocks_init();
    wntr_ticks_init();

    /* Enable the Micro Trace Buffer for better debug stacktraces. */
    wntr_mtb_init();

    return;
}

void SystemCoreClockUpdate(void) {
    /*
        Not implemented. According to CMSIS, this should evaluate the processor
        clock registers to determine the clock. However, it seems in practice
        that most programs just set the SystemCoreClock variable manually since
        it tends to stay constant after startup.
    */
}
