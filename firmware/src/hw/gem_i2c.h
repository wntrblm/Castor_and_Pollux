/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for interacting with I2C devices. */

#include "sam.h"
#include "wntr_gpio.h"
#include "wntr_ramfunc.h"
#include <stdbool.h>
#include <stddef.h>

struct GemI2CConfig {
    uint32_t gclk;
    uint32_t gclk_freq;
    uint32_t baudrate;
    uint32_t rise_time;
    uint32_t wait_timeout;
    Sercom* sercom;
    uint32_t apbcmask;
    uint32_t clkctrl_id;
    struct WntrGPIOPin pad0_pin;
    struct WntrGPIOPin pad1_pin;
};

enum GemI2CResult {
    GEM_I2C_RESULT_SUCCESS = 0,
    GEM_I2C_RESULT_ERR_ADDR_NACK = -1,
    GEM_I2C_RESULT_ERR_BUSSTATE = -2,
    GEM_I2C_RESULT_ERR_BUSERR = -3,
    GEM_I2C_RESULT_ERR_DATA_NACK = -4,
};

void gem_i2c_init(const struct GemI2CConfig* cfg);

RAMFUNC enum GemI2CResult gem_i2c_write(const struct GemI2CConfig* cfg, uint8_t address, uint8_t* data, size_t len);
