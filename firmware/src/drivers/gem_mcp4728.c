/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_mcp4728.h"
#include "gem_i2c.h"
#include "printf.h"

#define SINGLE_WRITE_CMD 0b01011000

static const uint8_t i2c_addresses_[] = {
    // MCP4728A0 used for rounds 1 & 2.
    0x60,
    // MCP4628A4 used for round 3.
    0x64,
};

static uint8_t address_ = 0;

void gem_mcp_4728_init(const struct GemI2CConfig* i2c) {
    for (size_t i = 0; i < sizeof(i2c_addresses_) / sizeof(i2c_addresses_[0]); i++) {
        address_ = i2c_addresses_[i];
        enum GemI2CResult result = gem_mcp_4728_write_channel(
            i2c,
            0,
            (struct GemMCP4278Channel){
                .value = 0,
            });

        if (result == GEM_I2C_RESULT_SUCCESS) {
            printf("MCP4728 found at 0x%02x.\n", address_);
            return;
        }
    }
    printf("Could not find MCP4728!\n");
}

enum GemI2CResult
gem_mcp_4728_write_channel(const struct GemI2CConfig* i2c, uint8_t channel_no, struct GemMCP4278Channel settings) {
    uint8_t data[3] = {
        SINGLE_WRITE_CMD | ((channel_no & 0x3) << 1),
        (settings.vref << 7) | (settings.pd << 6) | (settings.gain << 4) | ((settings.value >> 8) & 0xF),
        (settings.value & 0xFF),
    };

    return gem_i2c_write(i2c, address_, data, 3);
}

enum GemI2CResult gem_mcp_4728_write_channels(
    const struct GemI2CConfig* i2c,
    struct GemMCP4278Channel ch_a_settings,
    struct GemMCP4278Channel ch_b_settings,
    struct GemMCP4278Channel ch_c_settings,
    struct GemMCP4278Channel ch_d_settings) {
    uint8_t data[9] = {
        (ch_a_settings.pd << 4) | ((ch_a_settings.value >> 8) & 0xF),
        (ch_a_settings.value & 0xFF),
        (ch_b_settings.pd << 4) | ((ch_b_settings.value >> 8) & 0xF),
        (ch_b_settings.value & 0xFF),
        (ch_c_settings.pd << 4) | ((ch_c_settings.value >> 8) & 0xF),
        (ch_c_settings.value & 0xFF),
        (ch_d_settings.pd << 4) | ((ch_d_settings.value >> 8) & 0xF),
        (ch_d_settings.value & 0xFF),
    };

    return gem_i2c_write(i2c, address_, data, 9);
}
