/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
   A wrapper over bare SysEx messagse that allows mapping to specific "command"
   handlers.

   For example, given the SysEx message:

        0xF0 0x77 0x01 ... 0xF7

   The dispatcher considers this command 0x01 and dispatches to the handler
   registered at 0x01.

   The first byte after the SysEx start byte is the identifier byte (0x77 in
   the above example). This should be set using the GEM_SYSEX_IDENTIFIER
   macro.

   The command callbacks are called with just the data playload - it does not
   include the SysEx start byte, identifier byte, command byte, or SysEx end
   byte.
*/

#include <stddef.h>
#include <stdint.h>

/*
    A gem_midi_sysex_callback that will dispatch to specific commands
    registered. This should be wired up using gem_midi_set_sysex_callback();
*/
void gem_sysex_dispatcher(const uint8_t* data, size_t len);

/*
   Registers a callback for a particular sysex command.
   The data array passed in *does not* contain the SysEx start byte,
   but it *does* contain the SysEx end byte.
*/
typedef void (*gem_sysex_command_callback)(const uint8_t*, size_t);
void gem_sysex_register_command(uint8_t command, gem_sysex_command_callback callback);
