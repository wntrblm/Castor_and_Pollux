#pragma once
#include "gem_i2c.h"

enum gem_i2c_result gem_mcp_4728_write_channel(uint8_t channel, uint16_t value);
enum gem_i2c_result
gem_mcp_4728_write_channels(uint16_t ch_a_value, uint16_t ch_b_value, uint16_t ch_c_value, uint16_t ch_d_value);