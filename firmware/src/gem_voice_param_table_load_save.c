#include "gem_nvm.h"
#include "gem_voice_param_table.h"
#include "printf.h"
#include <stdbool.h>

#define VALID_TABLE_MARKER 0x41

#define UNPACK_16(data, idx) data[idx] << 8 | data[idx + 1]
#define PACK_16(val, data, idx)                                                                                        \
    data[idx] = val >> 8;                                                                                              \
    data[idx + 1] = val & 0xFF;

extern uint8_t _nvm_lut_base_address;
extern uint8_t _nvm_lut_length;

void gem_load_dac_codes_table() {
    uint8_t data[32];

    gem_nvm_read((uint32_t)(&_nvm_lut_base_address) + (uint32_t)(&_nvm_lut_length) - 1, data, 1);

    if (data[0] != VALID_TABLE_MARKER) {
        printf("No valid LUT table.\r\n");
        return;
    }

    size_t nvm_idx = 0;
    size_t table_idx = 0;
    bool done = false;

    while (nvm_idx < (uint32_t)(&_nvm_lut_length) && !done) {
        gem_nvm_read((uint32_t)(&_nvm_lut_base_address) + nvm_idx, data, 32);
        nvm_idx += 32;

        for (size_t i = 0; i < 32; i += 4) {
            if (table_idx > gem_voice_param_table_len) {
                done = true;
                break;
            }

            gem_voice_dac_codes_table[table_idx].castor = UNPACK_16(data, i);
            gem_voice_dac_codes_table[table_idx].pollux = UNPACK_16(data, i + 2);
        }
    }
}

void gem_save_dac_codes_table() {
    uint8_t data[32];

    size_t table_idx = 0;
    size_t nvm_idx = 0;
    bool done = false;

    while (nvm_idx < (uint32_t)(&_nvm_lut_length) && !done) {
        for (size_t i = 0; i < (32 / 4); i++) {
            if (table_idx + 1 > gem_voice_param_table_len) {
                done = true;
                break;
            }
            PACK_16(gem_voice_dac_codes_table[table_idx + i].castor, data, i * 4);
            PACK_16(gem_voice_dac_codes_table[table_idx + i].pollux, data, i * 4 + 2);
        }
        gem_nvm_write((uint32_t)(&_nvm_lut_base_address) + nvm_idx, data, 32);
        table_idx += (32 / 4);
        nvm_idx += 32;
    }
}