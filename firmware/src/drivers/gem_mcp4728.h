/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for controlling the MCP4728 quad 12-bit DAC over I2C. */

#include "gem_i2c.h"

struct GemMCP4278Channel {
    /* Voltage reference selection
        0: External vref (3.3v)
        1: Internal vref (2.048v)
    */
    uint8_t vref : 1;
    /* Power down mode
        0: Normal mode
        1: 1kΩ pull-down
        2: 100kΩ pull-down
        3: 500kΩ pull-down
    */
    uint8_t pd : 2;
    /* Gain selection
        0: 1x
        1: 2x
    */
    uint8_t gain : 1;
    /* Output code */
    uint16_t value : 12;
};

enum GemI2CResult gem_mcp_4728_write_channel(uint8_t channel_no, struct GemMCP4278Channel settings);
enum GemI2CResult gem_mcp_4728_write_channels(
    struct GemMCP4278Channel ch_a_settings,
    struct GemMCP4278Channel ch_b_settings,
    struct GemMCP4278Channel ch_c_settings,
    struct GemMCP4278Channel ch_d_settings);
