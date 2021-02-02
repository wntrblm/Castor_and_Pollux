/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_sysex_dispatcher.h"
#include "gem_config.h"
#include "printf.h"
#include "wntr_assert.h"
#include "wntr_midi_core.h"

/* Macros & definitions */

#define MAX_SYSEX_CALLBACKS 32

/* Static variables. */

static gem_sysex_command_callback sysex_callbacks_[MAX_SYSEX_CALLBACKS];

void gem_sysex_dispatcher(const uint8_t* data, size_t len) {
    /*
        Dispatchable SysEx messages must be at least 4 bytes long:

                       0xF0 0x?? 0x?? 0xF7
        start byte ────┘    │    │    |
        identifier ─────────┘    │    |
        command    ──────────────┘    │
        end byte   ───────────────────┘
    */
    if (len < 4) {
        printf("Invalid SysEx (too short): length: %lu\r\n", len);
        return;
    }

    if (data[1] != GEM_SYSEX_IDENTIFIER) {
        printf("Invalid SysEx (wrong marker byte): %02x, length: %lu\r\n", data[1], len);
        return;
    }

    uint8_t command = data[2];
    if (command >= MAX_SYSEX_CALLBACKS || sysex_callbacks_[command] == NULL) {
        printf("Invalid SysEx (invalid command): %02x\r\n", command);
        return;
    }

    /*
        Invoke the callback - remove the 3 header bytes and the trailing SysEx
        end byte.
    */
    sysex_callbacks_[command](data + 3, len - 4);
}

void gem_sysex_register_command(uint8_t command, gem_sysex_command_callback callback) {
    WNTR_ASSERT(command < MAX_SYSEX_CALLBACKS);
    sysex_callbacks_[command] = callback;
}
