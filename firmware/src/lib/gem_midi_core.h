#pragma once

/*
   Core MIDI system with handlers and callbacks for incoming MIDI messages.

   Presently this only really reacts to SysEx commands, since that's the
   only thing Gemini uses MIDI for.   
*/

#include <stddef.h>
#include <stdint.h>

enum gem_midi_message_types {
    MIDI_SYSEX_START_OR_CONTINUE = 0x04,
    MIDI_SYSEX_END_THREE_BYTE = 0x07,
    MIDI_SYSEX_END_TWO_BYTE = 0x06,
    MIDI_SYSEX_END_ONE_BYTE = 0x05,
    MIDI_SYSEX_START_BYTE = 0xF0,
    MIDI_SYSEX_END_BYTE = 0xF7,
};

/* Should be called at least once per millisecond to process outstanding
   MIDI messages. */
void gem_midi_task();

/* Encodes each byte into two bytes by splitting up the nibbles
   so the data can be transfered over MIDI SysEx.
   dst much be at least twice the length of src. */
void gem_midi_encode(uint8_t* src, uint8_t* dst, size_t src_len);

/* Decodes each pair of bytes as two nibbles into one byte.
   src must be twice the length of dst.*/
void gem_midi_decode(uint8_t* src, uint8_t* dst, size_t dst_len);

/* Sends a SysEx message, breaking the bytearray into the
   proper amount of MIDI messages. */
void gem_midi_send_sysex(uint8_t* data, size_t len);

/* Registers a callback for a particular sysex command.
   The data array passed in *does not* contain the SysEx start byte,
   but it *does* contain the SysEx end byte. */
typedef void (*gem_midi_sysex_command_callback)(uint8_t*, size_t);
void gem_midi_register_sysex_command(uint8_t command, gem_midi_sysex_command_callback callback);