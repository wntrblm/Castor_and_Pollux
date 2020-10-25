#pragma once

/* Routines for controlling the MCP4728 quad 12-bit DAC over I2C. */

#include "gem_i2c.h"

struct gem_mcp4728_channel {
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

enum gem_i2c_result gem_mcp_4728_write_channel(uint8_t channel_no, struct gem_mcp4728_channel settings);
enum gem_i2c_result gem_mcp_4728_write_channels(
    struct gem_mcp4728_channel ch_a_settings,
    struct gem_mcp4728_channel ch_b_settings,
    struct gem_mcp4728_channel ch_c_settings,
    struct gem_mcp4728_channel ch_d_settings);