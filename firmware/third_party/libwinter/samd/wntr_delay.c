/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_delay.h"

/*
    Adapated from CircuitPython:
    https://github.com/adafruit/circuitpython/blob/4d7b9cde33934a44c4c905a49d2f831339fc8bd2/ports/atmel-samd/mphalport.c#L52
*/

#ifdef SAMD21
#define DELAY_LOOP_ITERATIONS_PER_US 10U
#endif
#ifdef SAMD51
#define DELAY_LOOP_ITERATIONS_PER_US 30U
#endif

void wntr_delay_us(uint32_t microseconds) {
    for (uint32_t i = microseconds * DELAY_LOOP_ITERATIONS_PER_US; i > 0; i--) { asm volatile("nop"); }
}

void wntr_delay_ms(uint32_t milliseconds) { wntr_delay_us(milliseconds * 1000); }
