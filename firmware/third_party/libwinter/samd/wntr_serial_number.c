/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_serial_number.h"

/* From section 10.3.3 of the SAM D21 datasheet */
/* TODO: SAM D51 configuration. */
#define SERIAL_NO_WORD_0 (*((uint32_t*)0x0080A00C))
#define SERIAL_NO_WORD_1 (*((uint32_t*)0x0080A040))
#define SERIAL_NO_WORD_2 (*((uint32_t*)0x0080A044))
#define SERIAL_NO_WORD_3 (*((uint32_t*)0x0080A048))

void wntr_serial_number(uint8_t dst[WNTR_SERIAL_NUMBER_LEN]) {
    dst[0] = SERIAL_NO_WORD_0 >> 24 & 0xFF;
    dst[1] = SERIAL_NO_WORD_0 >> 16 & 0xFF;
    dst[2] = SERIAL_NO_WORD_0 >> 8 & 0xFF;
    dst[3] = SERIAL_NO_WORD_0 & 0xFF;
    dst[4] = SERIAL_NO_WORD_1 >> 24 & 0xFF;
    dst[5] = SERIAL_NO_WORD_1 >> 16 & 0xFF;
    dst[6] = SERIAL_NO_WORD_1 >> 8 & 0xFF;
    dst[7] = SERIAL_NO_WORD_1 & 0xFF;
    dst[8] = SERIAL_NO_WORD_2 >> 24 & 0xFF;
    dst[9] = SERIAL_NO_WORD_2 >> 16 & 0xFF;
    dst[10] = SERIAL_NO_WORD_2 >> 8 & 0xFF;
    dst[11] = SERIAL_NO_WORD_2 & 0xFF;
    dst[12] = SERIAL_NO_WORD_3 >> 24 & 0xFF;
    dst[13] = SERIAL_NO_WORD_3 >> 16 & 0xFF;
    dst[14] = SERIAL_NO_WORD_3 >> 8 & 0xFF;
    dst[15] = SERIAL_NO_WORD_3 & 0xFF;
}
