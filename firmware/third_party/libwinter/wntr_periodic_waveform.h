/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include <stddef.h>

/*
    Module for managing periodic waveforms.

    By specifying a frequency and a waveform function this module will handle
    updating the waveform's phase and value based on the amount of time that
    passes (as measured by SysTick).
*/

typedef fix16_t (*wntr_periodic_waveform_function)(fix16_t);

struct WntrPeriodicWaveform {
    wntr_periodic_waveform_function function;
    fix16_t frequency;
    fix16_t phase;
    uint32_t _last_update;
};

/*
    Configure a periodic waveform.

    The periodic waveform function should take the *phase* of the wave
    (0.0 -> 1.0) and return the waveform value.

    The frequency is in Hertz.

    In additional to this init method, you can modify the waveform's `phase`
    property when initializing the wave to make it phase shift compared to
    other periodic waveforms.
*/
void WntrPeriodicWaveform_init(
    struct WntrPeriodicWaveform* waveform,
    wntr_periodic_waveform_function function,
    fix16_t frequency,
    uint32_t start_time);

/*
    Update a periodic waveform given the current time (in milliseconds)
*/
fix16_t WntrPeriodicWaveform_step(struct WntrPeriodicWaveform* waveform, uint32_t time);

struct WntrMixedPeriodicWaveform {
    wntr_periodic_waveform_function* functions;
    size_t count;
    fix16_t* frequencies;
    fix16_t* factors;
    fix16_t* phases;
    uint32_t _last_update;
};

/*
    Like WntrPeriodicWaveform, but combines multiple waveforms together.
    The waveforms are multiplied by their respective factors and then all
    added together.
*/
void WntrMixedPeriodicWaveform_init(
    struct WntrMixedPeriodicWaveform* waveform,
    size_t count,
    wntr_periodic_waveform_function* functions,
    fix16_t* frequencies,
    fix16_t* factors,
    fix16_t* phases,
    uint32_t start_time);

fix16_t WntrMixedPeriodicWaveform_step(struct WntrMixedPeriodicWaveform* waveform, uint32_t time);
