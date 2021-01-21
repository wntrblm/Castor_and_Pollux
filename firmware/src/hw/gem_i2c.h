/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for interacting with I2C devices. */

#include "sam.h"
#include <stdbool.h>
#include <stddef.h>

enum GemI2CResult {
    GEM_I2C_RESULT_SUCCESS = 0,
    GEM_I2C_RESULT_ERR_ADDR_NACK = -1,
    GEM_I2C_RESULT_ERR_BUSSTATE = -2,
    GEM_I2C_RESULT_ERR_BUSERR = -3,
    GEM_I2C_RESULT_ERR_DATA_NACK = -4,
};

void gem_i2c_init();

enum GemI2CResult gem_i2c_write(uint8_t address, uint8_t* data, size_t len);
