/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_periodic_waveform.h"
#include "wntr_ticks.h"

/* Constants and macros. */

#define TICKS_PER_SECOND F16(1000)

/* Public functions */

void WntrPeriodicWaveform_init(
    struct WntrPeriodicWaveform* waveform, wntr_periodic_waveform_function function, fix16_t frequency) {
    waveform->function = function;
    waveform->frequency = frequency;
    waveform->phase = F16(0);
    waveform->value = F16(0);
    waveform->_last_update = wntr_ticks();
}

void WntrPeriodicWaveform_step(struct WntrPeriodicWaveform* waveform) {
    uint32_t now = wntr_ticks();
    uint32_t delta = now - waveform->_last_update;

    if (delta > 0) {
        waveform->phase += fix16_mul(fix16_div(waveform->frequency, TICKS_PER_SECOND), fix16_from_int(delta));

        if (waveform->phase > F16(1.0))
            waveform->phase = fix16_sub(waveform->phase, F16(1.0));

        waveform->_last_update = now;
    }

    waveform->value = waveform->function(waveform->phase);
}
