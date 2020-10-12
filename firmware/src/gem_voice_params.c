#include <math.h>
#include <stdbool.h>

#include "gem_voice_params.h"

/* Private function forward declarations. */

static int32_t _fix16_lerp_int(int64_t a, int64_t b, uint32_t frac);

static void _find_nearest_table_entries(
    const struct gem_voice_voltage_and_period* volt_table,
    struct gem_voice_dac_codes* dac_table,
    size_t table_len,
    fix16_t voltage,
    struct gem_voice_params* low_params,
    struct gem_voice_params* high_params);

/* Public functions. */

void gem_voice_params_from_cv(
    const struct gem_voice_voltage_and_period* volt_table,
    struct gem_voice_dac_codes* dac_table,
    size_t table_len,
    fix16_t voltage,
    struct gem_voice_params* out) {
    struct gem_voice_params low = {};
    struct gem_voice_params high = {};

    _find_nearest_table_entries(volt_table, dac_table, table_len, voltage, &low, &high);

    /* Special case; low table entry and voltage are the same, just use the low entry. */
    if (low.voltage_and_period.voltage == voltage) {
        (*out) = low;
        return;
    }

    fix16_t t;
    if (low.voltage_and_period.voltage == high.voltage_and_period.voltage) {
        t = fix16_from_int(1);
    } else {
        t = fix16_sdiv(
            fix16_ssub(voltage, low.voltage_and_period.voltage),
            fix16_ssub(low.voltage_and_period.voltage, low.voltage_and_period.voltage));
    }
    uint32_t t_int = (uint32_t)(t);

    out->voltage_and_period.voltage = voltage;
    out->voltage_and_period.period =
        _fix16_lerp_int(low.voltage_and_period.period, high.voltage_and_period.period, t_int << 16);
    out->dac_codes.castor = fix16_lerp16(low.dac_codes.castor, high.dac_codes.castor, t_int);
    out->dac_codes.pollux = fix16_lerp16(low.dac_codes.pollux, high.dac_codes.pollux, t_int);
};

/* Private functions. */

/* Similar to libfixmath's fix16_lerp64, but intentionally operates on integers instead of fix16s */
static int32_t _fix16_lerp_int(int64_t a, int64_t b, uint32_t frac) {
    int64_t result;
    result = (a * (0 - frac));
    result += (b * frac);
    result >>= 32;
    return (int32_t)result;
}

/* Used to find the two nearest table entries for doing linear interpolation.
   the table entries are defined in gem_voice_params_table.h.
*/
static void _find_nearest_table_entries(
    const struct gem_voice_voltage_and_period* volt_table,
    struct gem_voice_dac_codes* dac_table,
    size_t table_len,
    fix16_t voltage,
    struct gem_voice_params* low_params,
    struct gem_voice_params* high_params) {

    size_t low_idx = 0;
    size_t high_idx = 0;
    const struct gem_voice_voltage_and_period* low = &volt_table[0];
    const struct gem_voice_voltage_and_period* high = &volt_table[0];
    const struct gem_voice_voltage_and_period* current;
    bool found = false;

    for (size_t i = 0; i < table_len; i++) {
        current = &volt_table[i];
        if (current->voltage <= voltage && current->voltage >= low->voltage) {
            low = current;
            low_idx = i;
        }
        if (current->voltage > voltage) {
            high = current;
            high_idx = i;
            found = true;
            break;
        }
    }
    /*
        If we never found a value higher than
        the target, the the target is outside
        of the range of the list. Therefore,
        the highest close number is also the
        lowest close number.
    */
    if (!found) {
        high = low;
        high_idx = low_idx;
    }

    /* Now fill in the voice params. */
    low_params->voltage_and_period = (*low);
    low_params->dac_codes = dac_table[low_idx];
    high_params->voltage_and_period = (*high);
    high_params->dac_codes = dac_table[high_idx];
};