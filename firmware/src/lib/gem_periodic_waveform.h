#pragma once

#include "fix16.h"

/* Module for generating and tracking period waveforms. */

typedef fix16_t (*gem_periodic_waveform_function)(fix16_t);

struct GemPeriodicWaveform {
    gem_periodic_waveform_function function;
    fix16_t phase;
    fix16_t frequency;
    fix16_t value;
    uint32_t _last_update;
};

void GemPeriodicWaveform_init(
    struct GemPeriodicWaveform* waveform,
    gem_periodic_waveform_function function,
    fix16_t frequency);

void GemPeriodicWaveform_step(struct GemPeriodicWaveform* waveform);
