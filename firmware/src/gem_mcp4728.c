#include "gem_mcp4728.h"
#include "gem_i2c.h"

#define I2C_ADDRESS 0x60
#define SINGLE_WRITE_CMD 0b01011000
#define SEQUENTIAL_WRITE_CMD 0b01010000

enum gem_i2c_result gem_mcp_4728_write_channel(uint8_t channel_no, struct gem_mcp4728_channel settings) {
    uint8_t data[3] = {
        SINGLE_WRITE_CMD | ((channel_no & 0x3) << 1),
        (settings.vref << 7) | (settings.pd << 6) | (settings.gain << 4) | ((settings.value >> 8) & 0xF),
        (settings.value & 0xFF),
    };

    return gem_i2c_write(I2C_ADDRESS, data, 3);
}

enum gem_i2c_result gem_mcp_4728_write_channels(struct gem_mcp4728_channel ch_a_settings,
                                                struct gem_mcp4728_channel ch_b_settings,
                                                struct gem_mcp4728_channel ch_c_settings,
                                                struct gem_mcp4728_channel ch_d_settings) {
    uint8_t data[9] = {
        SEQUENTIAL_WRITE_CMD,
        (ch_a_settings.vref << 7) | (ch_a_settings.pd << 6) | (ch_a_settings.gain << 4) |
            ((ch_a_settings.value >> 8) & 0xF),
        (ch_a_settings.value & 0xFF),
        (ch_b_settings.vref << 7) | (ch_b_settings.pd << 6) | (ch_b_settings.gain << 4) |
            ((ch_b_settings.value >> 8) & 0xF),
        (ch_b_settings.value & 0xFF),
        (ch_c_settings.vref << 7) | (ch_c_settings.pd << 6) | (ch_c_settings.gain << 4) |
            ((ch_c_settings.value >> 8) & 0xF),
        (ch_c_settings.value & 0xFF),
        (ch_d_settings.vref << 7) | (ch_d_settings.pd << 6) | (ch_d_settings.gain << 4) |
            ((ch_d_settings.value >> 8) & 0xF),
        (ch_d_settings.value & 0xFF),
    };

    return gem_i2c_write(I2C_ADDRESS, data, 9);
}