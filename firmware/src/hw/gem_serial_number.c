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

void gem_get_serial_number(uint8_t buf[GEM_SERIAL_NUMBER_LEN]) {
    buf[0] = SERIAL_NO_WORD_0 >> 24 & 0xFF;
    buf[1] = SERIAL_NO_WORD_0 >> 16 & 0xFF;
    buf[2] = SERIAL_NO_WORD_0 >> 8 & 0xFF;
    buf[3] = SERIAL_NO_WORD_0 & 0xFF;
    buf[4] = SERIAL_NO_WORD_1 >> 24 & 0xFF;
    buf[5] = SERIAL_NO_WORD_1 >> 16 & 0xFF;
    buf[6] = SERIAL_NO_WORD_1 >> 8 & 0xFF;
    buf[7] = SERIAL_NO_WORD_1 & 0xFF;
    buf[8] = SERIAL_NO_WORD_2 >> 24 & 0xFF;
    buf[9] = SERIAL_NO_WORD_2 >> 16 & 0xFF;
    buf[10] = SERIAL_NO_WORD_2 >> 8 & 0xFF;
    buf[11] = SERIAL_NO_WORD_2 & 0xFF;
    buf[12] = SERIAL_NO_WORD_3 >> 24 & 0xFF;
    buf[13] = SERIAL_NO_WORD_3 >> 16 & 0xFF;
    buf[14] = SERIAL_NO_WORD_3 >> 8 & 0xFF;
    buf[15] = SERIAL_NO_WORD_3 & 0xFF;
}
