/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_periodic_waveform.h"

/* Constants and macros. */

#define MS_PER_SECOND F16(1000)

/* Public functions */

void WntrPeriodicWaveform_init(
    struct WntrPeriodicWaveform* waveform,
    wntr_periodic_waveform_function function,
    fix16_t frequency,
    uint32_t start_time) {
    waveform->function = function;
    waveform->frequency = frequency;
    waveform->phase = F16(0);
    waveform->_last_update = start_time;
}

fix16_t WntrPeriodicWaveform_step(struct WntrPeriodicWaveform* waveform, uint32_t time) {
    uint32_t delta = time - waveform->_last_update;

    if (delta > 0) {
        waveform->phase += fix16_mul(fix16_div(waveform->frequency, MS_PER_SECOND), fix16_from_int(delta));

        if (waveform->phase > F16(1.0))
            waveform->phase = fix16_sub(waveform->phase, F16(1.0));

        waveform->_last_update = time;
    }

    return waveform->function(waveform->phase);
}

void WntrMixedPeriodicWaveform_init(
    struct WntrMixedPeriodicWaveform* waveform,
    size_t count,
    wntr_periodic_waveform_function* functions,
    fix16_t* frequencies,
    fix16_t* factors,
    fix16_t* phases,
    uint32_t start_time) {
    waveform->count = count;
    waveform->functions = functions;
    waveform->frequencies = frequencies;
    waveform->factors = factors;
    waveform->phases = phases;
    waveform->_last_update = start_time;
    for (size_t n = 0; n < waveform->count; n++) { waveform->phases[n] = F16(0); }
}

fix16_t WntrMixedPeriodicWaveform_step(struct WntrMixedPeriodicWaveform* waveform, uint32_t time) {
    fix16_t delta = fix16_from_int(time - waveform->_last_update);
    waveform->_last_update = time;

    fix16_t accum = F16(0);

    for (size_t n = 0; n < waveform->count; n++) {
        waveform->phases[n] += fix16_mul(fix16_div(waveform->frequencies[n], MS_PER_SECOND), delta);

        if (waveform->phases[n] > F16(1.0))
            waveform->phases[n] = fix16_sub(waveform->phases[n], F16(1.0));

        fix16_t waveform_value = waveform->functions[n](waveform->phases[n]);
        accum = fix16_add(accum, fix16_mul(waveform->factors[n], waveform_value));
    }

    return accum;
}
