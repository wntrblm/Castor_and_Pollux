/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
   Core MIDI system.
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum USBMIDICodeIndexes {
    MIDI_CODE_INDEX_RESERVED_0x0 = 0x0,
    MIDI_CODE_INDEX_RESERVED_0x1 = 0x1,
    MIDI_CODE_INDEX_TWO_BYTE_COMMON = 0x2,
    MIDI_CODE_INDEX_THREE_BYTE_COMMON = 0x3,
    MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE = 0x4,
    MIDI_CODE_INDEX_SYSEX_END_ONE_BYTE = 0x5,
    MIDI_CODE_INDEX_SYSEX_END_TWO_BYTE = 0x6,
    MIDI_CODE_INDEX_SYSEX_END_THREE_BYTE = 0x7,
    MIDI_CODE_INDEX_NOTE_OFF = 0x8,
    MIDI_CODE_INDEX_NOTE_ON = 0x9,
    MIDI_CODE_INDEX_POLY_KEY_PRESSURE = 0xA,
    MIDI_CODE_INDEX_CONTROL_CHANGE = 0xB,
    MIDI_CODE_INDEX_PROGRAM_CHANGE = 0xC,
    MIDI_CODE_INDEX_CHANNEL_PRESSURE = 0xD,
    MIDI_CODE_INDEX_PITCH_BEND = 0xE,
    MIDI_CODE_INDEX_SINGLE_BYTE = 0xF
};

struct WntrMIDIMessage {
    uint8_t cable : 4;
    uint8_t code_index : 4;
    uint8_t status;
    uint8_t data_0;
    uint8_t data_1;
};

/* Receive a MIDI message.

Copies the received message into the given `msg`. If the message received was
a sysex message, this function will consume all messages until the end of the
sysex. Check for this by checking `msg->code_index == MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE`.
You can then fetch the sysex data and len using `wntr_midi_sysex_data()` and
`wntr_midi_sysex_len()`.

Returns: true if a message was received, false otherwise.
*/
bool wntr_midi_receive(struct WntrMIDIMessage* msg);

/* Send a MIDI message. */
void wntr_midi_send(const struct WntrMIDIMessage* msg);

/* The length of the last recieved sysex message. */
size_t wntr_midi_sysex_len();

/* The data to the last received sysex message.

Note that this is only guarenteed to be valid until the next call to
`wntr_midi_receive`.

*/
const uint8_t* wntr_midi_sysex_data();
void wntr_midi_send_sysex(const uint8_t* data, size_t len);
