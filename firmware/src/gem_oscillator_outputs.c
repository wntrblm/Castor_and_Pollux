/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include <math.h>
#include <stdbool.h>

#include "gem_lookup_tables.h"
#include "gem_oscillator.h"

/*
    Uses the look-up tables to calculate the timer period and ramp control
    voltage for a given pitch control voltage input.
*/

/* Forward declarations. */

static int32_t fix16_lerp_int64_(int64_t a, int64_t b, uint16_t frac);

static void find_nearest_pair_with_pitch_cv_(
    uint8_t osc, fix16_t input, struct GemOscillatorOutputs* low_out, struct GemOscillatorOutputs* high_out);

static void find_nearest_pair_with_period_(
    uint8_t osc, uint32_t input, struct GemOscillatorOutputs* low_out, struct GemOscillatorOutputs* high_out);

/* Public functions. */

void GemOscillatorOutputs_calculate(uint8_t osc, fix16_t pitch_cv, struct GemOscillatorOutputs* out) {
    struct GemOscillatorOutputs low = {};
    struct GemOscillatorOutputs high = {};

    /*
        First step is to figure out the timer period needed for the given
        control voltage.

        This is done using a lookup table and linear interpolation. This seems
        to be faster than using the formula directly since the formula requires
        a power function (see oscillators.py).
    */

    find_nearest_pair_with_pitch_cv_(osc, pitch_cv, &low, &high);

    /*
        Before using lerp you have to figure out where the real value is in relation to
        the low and high values so that lerp knows how far to go- basically:

            frac = (value - low) / (high - low)

        `frac` is then used to interpolate between high and low:

            result = low + (high - low) * frac

        Since "frac" is necessarily a real number between 0 and 1, you'd expect
        that we'd use floats/fix16 here. However, we have lerp functions that
        take `frac` as a 16-bit value between 0 - 65355 (representing 0.0 and 1.0)

        Voltage happens to be a fix16 value already, so we could use fix16 math
        to determine the frac. However, since we know for sure that the dividend
        and divisor are always less than 1.0 we can skip fix16's logic and just
        use integer division which is a bit faster.
    */
    uint16_t voltage_frac_int;
    if (low.pitch_cv == high.pitch_cv) {
        voltage_frac_int = 65535;
    } else {
        uint32_t dividend = (uint32_t)(pitch_cv - low.pitch_cv);
        dividend <<= 16;
        uint32_t divisor = (uint32_t)(high.pitch_cv - low.pitch_cv);
        voltage_frac_int = (uint16_t)(dividend / divisor);
    }

    out->pitch_cv = pitch_cv;
    out->period = fix16_lerp_int64_(low.period, high.period, voltage_frac_int);

    /*
        Now figure out the DAC charge code.

        Just like with the timer period, this is done using a look-up table
        and interpolation. The lookup table for DAC codes is a bit smaller
        than the one for voltage & period. Also note that the period is sorted
        in decreasing order in the table, so this looks inverted.

        Also of note is that since this uses the period register instead of the
        frequency it isn't *quite* linear with frequency, but it's close enough.
    */
    find_nearest_pair_with_period_(osc, out->period, &low, &high);

    uint16_t dac_frac_int;
    if (low.period == high.period) {
        dac_frac_int = 65535;
    } else {
        /*
            Throw away some LSBs to avoid 64-bit division. There's still plenty
            of resolution to work with.
        */
        uint32_t dividend = ((uint32_t)(low.period - out->period)) >> 2;
        dividend <<= 16;
        uint32_t divisor = ((uint32_t)(low.period - high.period)) >> 2;
        dac_frac_int = (uint16_t)(dividend / divisor);
    }

    out->ramp_cv = fix16_lerp16(low.ramp_cv, high.ramp_cv, dac_frac_int);
};

/* Private functions. */

/* Similar to libfixmath's fix16_lerp64, but intentionally operates on integers instead of fix16s */
static int32_t fix16_lerp_int64_(int64_t a, int64_t b, uint16_t frac) {
    int64_t result;
    result = a * (((int32_t)1 << 16) - frac);
    result += b * frac;
    result >>= 16;
    return (int32_t)result;
}

#define FIND_NEAREST_PAIR(name, table, input_typename, entry_typename, compare_low, compare_high, assign)              \
    static void find_nearest_pair_with_##name##_(                                                                      \
        uint8_t osc,                                                                                                   \
        input_typename input,                                                                                          \
        struct GemOscillatorOutputs* low_out,                                                                          \
        struct GemOscillatorOutputs* high_out) {                                                                       \
        const entry_typename* low = &table[0];                                                                         \
        const entry_typename* high = &table[0];                                                                        \
        const entry_typename* current;                                                                                 \
        bool found = false;                                                                                            \
                                                                                                                       \
        for (size_t i = 0; i < table##_len; i++) {                                                                     \
            current = &table[i];                                                                                       \
            if (compare_low) {                                                                                         \
                low = current;                                                                                         \
            }                                                                                                          \
            if (compare_high) {                                                                                        \
                high = current;                                                                                        \
                found = true;                                                                                          \
                break;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
        if (!found) {                                                                                                  \
            high = low;                                                                                                \
        }                                                                                                              \
        assign(osc, low_out, low);                                                                                     \
        assign(osc, high_out, high);                                                                                   \
    }

static void assign_pitch_cv_(uint8_t osc, struct GemOscillatorOutputs* out, const struct GemPitchTableEntry* in) {
    (void)(osc);
    out->pitch_cv = in->pitch_cv;
    out->period = in->period;
}

FIND_NEAREST_PAIR(
    pitch_cv,
    gem_pitch_table,
    fix16_t,
    struct GemPitchTableEntry,
    (current->pitch_cv <= input && current->pitch_cv >= low->pitch_cv),
    (current->pitch_cv > input),
    assign_pitch_cv_);

static void assign_ramp_cv_(uint8_t osc, struct GemOscillatorOutputs* out, const struct GemRampTableEntry* in) {
    out->period = in->period;
    if (osc == 0) {
        out->ramp_cv = in->castor_ramp_cv;
    } else {
        out->ramp_cv = in->pollux_ramp_cv;
    }
}

FIND_NEAREST_PAIR(
    period,
    gem_ramp_table,
    uint32_t,
    struct GemRampTableEntry,
    (current->period >= input),
    (current->period < input && current->period <= low->period),
    assign_ramp_cv_);
