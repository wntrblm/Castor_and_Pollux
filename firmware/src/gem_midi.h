#pragma once

enum gem_midi_event {
    /* Signals that calibration mode should start. */
    GEM_MIDI_EVENT_CALIBRATION_MODE,
};

typedef void (*gem_midi_event_callback)(enum gem_midi_event);

void gem_midi_set_event_callback(gem_midi_event_callback callback);
void gem_midi_task();