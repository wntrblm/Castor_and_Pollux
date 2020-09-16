#include "gem_systick.h"
#include <sam.h>

uint32_t _ms_ticks;

void gem_systick_init() {
    /* Our system clock runs at 48 MHz, so this is 1ms per tick. */
    SysTick_Config(48000000 / 1000);
}

uint32_t gem_get_ticks() { return _ms_ticks; }

void SysTick_Handler(void) { _ms_ticks++; }