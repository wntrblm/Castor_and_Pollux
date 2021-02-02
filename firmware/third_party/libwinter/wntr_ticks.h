/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for timekeeping.

    This is implemented using the ARM SysTick peripheral.
*/

#include <stdint.h>

void wntr_ticks_init();

/*
    Returns the number of ticks (milliseconds) since the program started.
*/
uint32_t wntr_ticks();
