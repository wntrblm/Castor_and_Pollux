/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for timekeeping using the ARM SysTick peripheral. */

#include <stdint.h>

void gem_systick_init();

/* Returns the number of ticks (ms) that the device has been running. */
uint32_t gem_get_ticks();
