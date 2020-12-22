#pragma once

#include "fix16.h"

/* Module for generating and tracking period waveforms. */

typedef fix16_t (*gem_periodic_waveform_function)(fix16_t);

struct gem_periodic_waveform {
    gem_periodic_waveform_function function;
    fix16_t phase;
    fix16_t frequency;
    uint32_t _last_update;
};

void gem_periodic_waveform_init(
    struct gem_periodic_waveform* waveform,
    gem_periodic_waveform_function function,
    fix16_t frequency);

fix16_t gem_periodic_waveform_step(struct gem_periodic_waveform* waveform);