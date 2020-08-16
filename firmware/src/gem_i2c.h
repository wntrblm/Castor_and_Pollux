#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "sam.h"

enum gem_i2c_result {
    GEM_I2C_RESULT_SUCCESS = 0,
    GEM_I2C_RESULT_ERR_ADDR_NACK = -1,
    GEM_I2C_RESULT_ERR_BUSSTATE = -2,
    GEM_I2C_RESULT_ERR_BUSERR = -3,
    GEM_I2C_RESULT_ERR_DATA_NACK = -4,
};

void gem_i2c_init();

enum gem_i2c_result gem_i2c_write(uint8_t address, uint8_t* data, size_t len);