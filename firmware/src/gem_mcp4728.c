#include "gem_mcp4728.h"
#include "gem_i2c.h"

#define I2C_ADDRESS 0x60
#define SINGLE_WRITE_CMD 0b01011000
#define SEQUENTIAL_WRITE_CMD 0b01010000

enum gem_i2c_result gem_mcp_4728_write_channel(uint8_t channel, uint16_t value) {
    uint8_t data[3] = {
        SINGLE_WRITE_CMD | ((channel & 0x3) << 1),
        (value >> 8) & 0xF,
        (value & 0xFF),
    };

    return gem_i2c_write(I2C_ADDRESS, data, 3);
}

enum gem_i2c_result
gem_mcp_4728_write_channels(uint16_t ch_a_value, uint16_t ch_b_value, uint16_t ch_c_value, uint16_t ch_d_value) {
    uint8_t data[9] = {
        SEQUENTIAL_WRITE_CMD,
        (ch_a_value >> 8) & 0xF,
        (ch_a_value & 0xFF),
        (ch_b_value >> 8) & 0xF,
        (ch_b_value & 0xFF),
        (ch_c_value >> 8) & 0xF,
        (ch_c_value & 0xFF),
        (ch_d_value >> 8) & 0xF,
        (ch_d_value & 0xFF),
    };

    return gem_i2c_write(I2C_ADDRESS, data, 9);
}