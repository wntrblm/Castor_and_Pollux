/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
   MIDI helpers and utilities.
*/

#include "wntr_midi_core.h"

enum WntrMIDIChannelMessages {
    MIDI_CHANNEL_NOTE_OFF = 0x80,
    MIDI_CHANNEL_NOTE_ON = 0x90,
    MIDI_CHANNEL_POLY_KEY_PRESSURE = 0xA0,
    MIDI_CHANNEL_CONTROLLER_CHANGE = 0xB0,
    MIDI_CHANNEL_PROGRAM_CHANGE = 0xC0,
    MIDI_CHANNEL_CHANNEL_PRESSURE = 0xD0,
    MIDI_CHANNEL_PITCH_BEND = 0xE0,
};

enum WntrMIDICommonMessages {
    MIDI_COMMON_SYSEX = 0xF0,
    MIDI_COMMON_TIME_CODE = 0xF1,
    MIDI_COMMON_SONG_POSITION = 0xF2,
    MIDI_COMMON_SONG_SELECT = 0xF3,
    MIDI_COMMON_TUNE_REQUEST = 0xF6,
};

enum WntrMIDIRealtimeMessages {
    MIDI_REALTIME_CLOCK = 0xF8,
    MIDI_REALTIME_START = 0xFA,
    MIDI_REALTIME_CONTINUE = 0xFB,
    MIDI_REALTIME_STOP = 0xFC,
    MIDI_REALTIME_ACTIVE_SENSING = 0xFE,
    MIDI_REALTIME_SYSTEM_RESET = 0xFF,
};

inline static uint8_t wntr_midi_get_type(struct WntrMIDIMessage* msg) { return msg->status & 0xF0; }
inline static uint8_t wntr_midi_get_channel(struct WntrMIDIMessage* msg) { return msg->status & 0xF; }

inline static uint16_t wntr_midi_get_pitch_bend(struct WntrMIDIMessage* msg) {
    return (msg->data_1 << 7 | msg->data_0);
}

inline static uint16_t wntr_midi_get_pitch_bend_u16(struct WntrMIDIMessage* msg) {
    return ((msg->data_1 << 7 | msg->data_0) << 2);
}
