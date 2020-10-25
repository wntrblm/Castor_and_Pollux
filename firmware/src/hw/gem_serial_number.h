#pragma once

/* Routines for reading the unique device ID from the SAM D21. */

#include <stdint.h>

uint32_t gem_serial_number_high();
uint32_t gem_serial_number_low();