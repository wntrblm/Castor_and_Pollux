/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_midi_sysex_dispatcher.h"
#include "printf.h"
#include "wntr_assert.h"
#include "wntr_midi_core.h"

/* Static variables. */

static wntr_midi_sysex_command_callback sysex_callbacks_[WNTR_MIDI_MAX_SYSEX_CALLBACKS];

void wntr_midi_dispatch_sysex() {
    size_t len = wntr_midi_sysex_len();
    const uint8_t* data = wntr_midi_sysex_data();

    /*
        Dispatchable SysEx messages must be at least 2 bytes long:

                       0x?? 0x??
        identifier ─────┘    │
        command    ──────────┘
    */
    if (len < 2) {
        printf("Invalid SysEx (too short): length: %lu\r\n", len);
        return;
    }

    if (data[0] != WNTR_MIDI_SYSEX_IDENTIFIER) {
        printf("Invalid SysEx (wrong marker byte): 0x%02x, length: %lu\r\n", data[0], len);
        return;
    }

    uint8_t command = data[1];
    if (command >= WNTR_MIDI_MAX_SYSEX_CALLBACKS || sysex_callbacks_[command] == NULL) {
        printf("Invalid SysEx (invalid command): %02x\r\n", command);
        return;
    }

    /*
        Invoke the callback - remove the 2 header bytes.
    */
    sysex_callbacks_[command](data + 2, len - 2);
}

void wntr_midi_register_sysex_command(uint8_t command, wntr_midi_sysex_command_callback callback) {
    WNTR_ASSERT(command < WNTR_MIDI_MAX_SYSEX_CALLBACKS);
    sysex_callbacks_[command] = callback;
}
