#pragma once

#include <stddef.h>
#include <stdint.h>

enum gem_sysex_event {
    /* Signals that calibration mode should start. */
    GEM_SYSEX_EVENT_CALIBRATION_MODE,
};

typedef void (*gem_sysex_event_callback)(enum gem_sysex_event);

void gem_set_sysex_event_callback(gem_sysex_event_callback callback);

void gem_process_sysex(uint8_t* data);