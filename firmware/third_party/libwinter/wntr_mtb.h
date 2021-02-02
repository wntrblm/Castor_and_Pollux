/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Methods for enabling the Micro Trace Buffer to aid with debugging. The
    MTB allows tracing the program's execution at the processor level. This
    can often help obtain a usable stacktrace/backtrace when debugging in
    interrupts.

    The Micro Trace Buffer can be read using GDB using a python script:

    > source third_party/mtb/micro-trace-buffer.py
    > mtb

    References:
    * https://learn.adafruit.com/debugging-the-samd21-with-gdb/micro-trace-buffer
    * https://developer.arm.com/documentation/ddi0486/b
    * https://github.com/adafruit/gdb-micro-trace-buffer
*/

/*
    Initializes the Micro Trace Buffer and enables it.
*/
void wntr_mtb_init();

/*
    Disables the Micro Trace Buffer. This is useful before entering long or
    infinite loops as it'll prevent the MTB from being filled with useless
    data.
*/
void wntr_mtb_disable();
void wntr_mtb_enable();
