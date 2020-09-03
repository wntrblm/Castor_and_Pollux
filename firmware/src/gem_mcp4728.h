#pragma once
#include "gem_i2c.h"

struct gem_mcp4728_channel {
    uint8_t vref:1;
    uint8_t pd:2;
    uint8_t gain:1;
    uint16_t value:12;
};

enum gem_i2c_result gem_mcp_4728_write_channel(uint8_t channel_no, struct gem_mcp4728_channel settings);
enum gem_i2c_result
gem_mcp_4728_write_channels(struct gem_mcp4728_channel ch_a_settings, struct gem_mcp4728_channel ch_b_settings, struct gem_mcp4728_channel ch_c_settings, struct gem_mcp4728_channel ch_d_settings);