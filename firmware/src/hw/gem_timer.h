/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for using the SAM D21 TC peripheral as a general-purpose
    timer interrupt.
*/

#include "wntr_ramfunc.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*gem_timer_callback)(void);

void gem_timer_init(gem_timer_callback callback);
