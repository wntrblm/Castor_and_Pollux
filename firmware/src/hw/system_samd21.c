#include "samd21.h"

/*
    Initial clock frequency on the SAMD21 is 1 MHz.
*/
#define STARTUP_SYSTEM_CLOCK (1000000)

uint32_t SystemCoreClock = STARTUP_SYSTEM_CLOCK;

void SystemInit(void) {
    SystemCoreClock = STARTUP_SYSTEM_CLOCK;
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
