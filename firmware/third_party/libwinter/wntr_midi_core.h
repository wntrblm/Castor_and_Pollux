/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
   Core MIDI system with handlers and callbacks for incoming MIDI messages.

   Presently this only really reacts to SysEx commands, since that's the
   only thing Gemini uses MIDI for.
*/

#include <stddef.h>
#include <stdint.h>

/*
   Should be called at least once per millisecond to process outstanding
   MIDI messages.
*/
void wntr_midi_task();

/*
   Register a callback to handle SysEx messages.
*/
typedef void (*wntr_midi_sysex_callback)(const uint8_t*, size_t);
void wntr_midi_set_sysex_callback(wntr_midi_sysex_callback callback);

/*
   Send a sysex message. The data should not contain the start and end byte,
   this function will add it automatically.
*/
void wntr_midi_send_sysex(const uint8_t* data, size_t len);
