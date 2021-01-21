/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_serial_number.h"

#define SERIAL_NO_WORD_0 (*((uint32_t*)0x0080A00C))
#define SERIAL_NO_WORD_1 (*((uint32_t*)0x0080A040))
#define SERIAL_NO_WORD_2 (*((uint32_t*)0x0080A044))
#define SERIAL_NO_WORD_3 (*((uint32_t*)0x0080A048))

uint32_t gem_serial_number_high() { return SERIAL_NO_WORD_0 << 16 | SERIAL_NO_WORD_1; }

uint32_t gem_serial_number_low() { return SERIAL_NO_WORD_2 << 16 | SERIAL_NO_WORD_3; }
