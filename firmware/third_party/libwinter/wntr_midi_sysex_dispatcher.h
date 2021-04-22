/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
   A wrapper over bare SysEx messages that allows mapping to specific "command"
   handlers.

   For example, given the SysEx message:

        0xF0 0x77 0x01 ... 0xF7

   The dispatcher considers this command 0x01 and dispatches to the handler
   registered at 0x01.

   The first byte after the SysEx start byte is the identifier byte (0x77 in
   the above example). This should be set using the WNTR_MIDI_SYSEX_IDENTIFIER
   macro.

   The command callbacks are called with just the data playload - it does not
   include the SysEx start byte, identifier byte, command byte, or SysEx end
   byte.
*/

#include <stddef.h>
#include <stdint.h>

#ifndef WNTR_MIDI_MAX_SYSEX_CALLBACKS
#define WNTR_MIDI_MAX_SYSEX_CALLBACKS 32
#endif

#ifndef WNTR_MIDI_SYSEX_IDENTIFIER
/*
    Each SysEx command must start with this identifier byte, otherwise, the
    command is discarded. This helps avoid trying to parse SysEx messages that
    weren't intended for this device.
*/
#define WNTR_MIDI_SYSEX_IDENTIFIER 0x1B
#endif

/*
    A wntr_midi_sysex_callback that will dispatch to specific commands
    registered. This should be wired up using wntr_midi_set_sysex_callback();
*/
void wntr_midi_dispatch_sysex();

/*
   Registers a callback for a particular sysex command.
   The data array passed in *does not* contain the SysEx start byte,
   but it *does* contain the SysEx end byte.
*/
typedef void (*wntr_midi_sysex_command_callback)(const uint8_t*, size_t);
void wntr_midi_register_sysex_command(uint8_t command, wntr_midi_sysex_command_callback callback);
