/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Methods for resetting into the bootloader (if it's available)
*/

void wntr_reset_into_bootloader() __attribute__((__noreturn__));
