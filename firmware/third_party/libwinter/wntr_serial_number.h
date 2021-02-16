/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for reading the unique device ID/serial number embedded in the CPU. */

#include <stdint.h>

#define WNTR_SERIAL_NUMBER_LEN 16

void wntr_serial_number(uint8_t dst[WNTR_SERIAL_NUMBER_LEN]);
