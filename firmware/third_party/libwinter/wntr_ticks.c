/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_ticks.h"
#include <sam.h>

/*
    This gives just under 50 days before the timer overflows. I currently don't
    have any device that's intended to run for 50 days continously.
*/
uint32_t _ms_ticks;

void wntr_ticks_init() {
    /* 1ms per tick. */
    SysTick_Config(SystemCoreClock / 1000);
}

uint32_t wntr_ticks() { return _ms_ticks; }

void SysTick_Handler(void) { _ms_ticks++; }
