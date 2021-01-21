/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for reading the unique device ID from the SAM D21. */

#include <stdint.h>

#define GEM_SERIAL_NUMBER_LEN 16

uint32_t gem_serial_number_high();
uint32_t gem_serial_number_low();
void gem_get_serial_number(uint8_t buf[GEM_SERIAL_NUMBER_LEN]);
