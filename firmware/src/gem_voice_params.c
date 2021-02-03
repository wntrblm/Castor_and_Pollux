/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include <math.h>
#include <stdbool.h>

#include "gem_voice_param_table.h"
#include "gem_voice_params.h"

/* Private function forward declarations. */

static int32_t fix16_lerp_int64_(int64_t a, int64_t b, uint16_t frac);

static void find_nearest_pair_with_voltage_(
    fix16_t input, struct GemVoltageAndPeriod* low_out, struct GemVoltageAndPeriod* high_out);

static void
find_nearest_pair_with_period_(uint32_t input, struct GemDACCodePair* low_out, struct GemDACCodePair* high_out);

/* Public functions. */

void GemVoiceParams_from_cv(fix16_t pitch_control_voltage, struct GemVoiceParams* out) {
    struct GemVoiceParams low = {};
    struct GemVoiceParams high = {};

    /*
        First step is to figure out the timer period needed for the given
        control voltage.

        This is done using a lookup table and linear interpolation. This seems
        to be faster than using the formula directly since the formula requires
        a power function (see oscillators.py).
    */

    find_nearest_pair_with_voltage_(pitch_control_voltage, &low.voltage_and_period, &high.voltage_and_period);

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
    if (low.voltage_and_period.voltage == high.voltage_and_period.voltage) {
        voltage_frac_int = 65535;
    } else {
        uint32_t dividend = (uint32_t)(pitch_control_voltage - low.voltage_and_period.voltage);
        dividend <<= 16;
        uint32_t divisor = (uint32_t)(high.voltage_and_period.voltage - low.voltage_and_period.voltage);
        voltage_frac_int = (uint16_t)(dividend / divisor);
    }

    out->voltage_and_period.voltage = pitch_control_voltage;
    out->voltage_and_period.period =
        fix16_lerp_int64_(low.voltage_and_period.period, high.voltage_and_period.period, voltage_frac_int);

    /*
        Now figure out the DAC charge code.

        Just like with the timer period, this is done using a look-up table
        and interpolation. The lookup table for DAC codes is a bit smaller
        than the one for voltage & period. Also note that the period is sorted
        in decreasing order in the table, so this looks inverted.
    */
    find_nearest_pair_with_period_(out->voltage_and_period.period, &low.dac_codes, &high.dac_codes);

    uint16_t dac_frac_int;
    if (low.dac_codes.period == high.dac_codes.period) {
        dac_frac_int = 65535;
    } else {
        /*
            Throw away some LSBs to avoid 64-bit division. There's still plenty
            of resolution to work with.
        */
        uint32_t dividend = ((uint32_t)(low.dac_codes.period - out->voltage_and_period.period)) >> 2;
        dividend <<= 16;
        uint32_t divisor = ((uint32_t)(low.dac_codes.period - high.dac_codes.period)) >> 2;
        dac_frac_int = (uint16_t)(dividend / divisor);
    }

    out->dac_codes.period = out->voltage_and_period.period;
    out->dac_codes.castor = fix16_lerp16(low.dac_codes.castor, high.dac_codes.castor, dac_frac_int);
    out->dac_codes.pollux = fix16_lerp16(low.dac_codes.pollux, high.dac_codes.pollux, dac_frac_int);
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

#define FIND_NEAREST_PAIR(name, table, input_typename, entry_typename, compare_low, compare_high)                      \
    static void find_nearest_pair_with_##name##_(                                                                      \
        input_typename input, entry_typename* low_out, entry_typename* high_out) {                                     \
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
        (*low_out) = (*low);                                                                                           \
        (*high_out) = (*high);                                                                                         \
    }

FIND_NEAREST_PAIR(
    voltage,
    gem_voice_voltage_and_period_table,
    fix16_t,
    struct GemVoltageAndPeriod,
    (current->voltage <= input && current->voltage >= low->voltage),
    (current->voltage > input));

FIND_NEAREST_PAIR(
    period,
    gem_voice_dac_codes_table,
    uint32_t,
    struct GemDACCodePair,
    (current->period >= input),
    (current->period < input && current->period <= low->period));
