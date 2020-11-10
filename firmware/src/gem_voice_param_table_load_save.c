#include "gem_nvm.h"
#include "gem_pack.h"
#include "gem_voice_param_table.h"
#include "printf.h"
#include <stdbool.h>

#define VALID_TABLE_MARKER 0x41
#define BUFFER_LEN 512

extern uint8_t _nvm_lut_base_address;
extern uint8_t _nvm_lut_length;

/* This is inefficient in terms of RAM usage, but we've got plenty of space to spare.
   if in the future this becomes problematic, this can be refactored to read/write
   in pages instead of all at once.
*/
static uint8_t _buf[BUFFER_LEN];

/* Public functions. */

void gem_load_dac_codes_table() {
    /* Make sure these are equivalent - otherwise bail. */
    if ((uint32_t)(&_nvm_lut_length) != sizeof(_buf) / sizeof(_buf[0])) {
        printf("NVM LUT length is not equal to the NVM LUT buffer!\r\n");
        return;
    }

    gem_nvm_read((uint32_t)(&_nvm_lut_base_address), _buf, BUFFER_LEN);

    if (_buf[BUFFER_LEN - 1] != VALID_TABLE_MARKER) {
        printf("No valid LUT table.\r\n");
        return;
    }

    uint16_t checksum = 0;
    for (size_t table_idx = 0; table_idx < gem_voice_param_table_len; table_idx++) {
        gem_voice_dac_codes_table[table_idx].castor = UNPACK_16(_buf, table_idx * 4);
        gem_voice_dac_codes_table[table_idx].pollux = UNPACK_16(_buf, table_idx * 4 + 2);

        checksum ^= gem_voice_dac_codes_table[table_idx].castor;
    }

    printf("LUT table loaded from NVM, checksum: %04x\r\n", checksum);
}

void gem_save_dac_codes_table() {
    for (size_t table_idx = 0; table_idx < gem_voice_param_table_len; table_idx++) {
        PACK_16(gem_voice_dac_codes_table[table_idx].castor, _buf, table_idx * 4);
        PACK_16(gem_voice_dac_codes_table[table_idx].pollux, _buf, table_idx * 4 + 2);
    }

    _buf[BUFFER_LEN - 1] = VALID_TABLE_MARKER;

    gem_nvm_write((uint32_t)(&_nvm_lut_base_address), _buf, BUFFER_LEN);
}