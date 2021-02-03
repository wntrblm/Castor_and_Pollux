/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_nvm.h"
#include "gem_voice_param_table.h"
#include "printf.h"
#include "wntr_pack.h"
#include <stdbool.h>
#include <string.h>

#define VALID_TABLE_MARKER 0x43
#define BUFFER_LEN 512

extern uint8_t _nvm_lut_base_address;
extern uint8_t _nvm_lut_length;

/* This is inefficient in terms of RAM usage, but we've got plenty of space to spare.
   if in the future this becomes problematic, this can be refactored to read/write
   in pages instead of all at once.
*/
static uint8_t param_table_load_buf_[BUFFER_LEN];

/* Public functions. */

void gem_load_dac_codes_table() {
    /* Make sure these are equivalent - otherwise bail. */
    if ((uint32_t)(&_nvm_lut_length) != sizeof(param_table_load_buf_) / sizeof(param_table_load_buf_[0])) {
        printf("NVM LUT length is not equal to the NVM LUT buffer!\r\n");
        return;
    }

    gem_nvm_read((uint32_t)(&_nvm_lut_base_address), param_table_load_buf_, BUFFER_LEN);

    if (param_table_load_buf_[BUFFER_LEN - 1] != VALID_TABLE_MARKER) {
        printf("No valid LUT table.\r\n");
        return;
    }

    uint16_t checksum = 0;
    for (size_t table_idx = 0; table_idx < gem_voice_dac_codes_table_len; table_idx++) {
        gem_voice_dac_codes_table[table_idx].period = WNTR_UNPACK_32(param_table_load_buf_, table_idx * 4);
        gem_voice_dac_codes_table[table_idx].castor = WNTR_UNPACK_16(param_table_load_buf_, table_idx * 4 + 4);
        gem_voice_dac_codes_table[table_idx].pollux = WNTR_UNPACK_16(param_table_load_buf_, table_idx * 4 + 6);

        checksum ^= gem_voice_dac_codes_table[table_idx].castor;
    }

    printf("LUT table loaded from NVM, checksum: %04x\r\n", checksum);
}

void gem_save_dac_codes_table() {
    for (size_t table_idx = 0; table_idx < gem_voice_dac_codes_table_len; table_idx++) {
        WNTR_PACK_32(gem_voice_dac_codes_table[table_idx].period, param_table_load_buf_, table_idx * 4)
        WNTR_PACK_16(gem_voice_dac_codes_table[table_idx].castor, param_table_load_buf_, table_idx * 4 + 4);
        WNTR_PACK_16(gem_voice_dac_codes_table[table_idx].pollux, param_table_load_buf_, table_idx * 4 + 6);
    }

    param_table_load_buf_[BUFFER_LEN - 1] = VALID_TABLE_MARKER;

    gem_nvm_write((uint32_t)(&_nvm_lut_base_address), param_table_load_buf_, BUFFER_LEN);
}

void gem_erase_dac_codes_table() {
    memset(param_table_load_buf_, 0xFF, BUFFER_LEN);
    gem_nvm_write((uint32_t)(&_nvm_lut_base_address), param_table_load_buf_, BUFFER_LEN);
}
