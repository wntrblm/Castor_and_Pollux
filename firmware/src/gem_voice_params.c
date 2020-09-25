#include <math.h>
#include <stdbool.h>

#include "gem_voice_params.h"

/* Private function forward declarations. */

static int32_t _fix16_lerp_int(int64_t a, int64_t b, uint32_t frac);

static void _find_nearest_table_entries(
    struct gem_voice_params* param_table,
    size_t table_len,
    fix16_t voltage,
    struct gem_voice_params** low,
    struct gem_voice_params** high);

/* Public functions. */

void gem_voice_params_from_cv(
    struct gem_voice_params* table, size_t table_len, fix16_t voltage, struct gem_voice_params* out) {
    struct gem_voice_params* low;
    struct gem_voice_params* high;

    _find_nearest_table_entries(table, table_len, voltage, &low, &high);

    /* Special case; low table entry and voltage are the same, just use the low entry. */
    if (low->voltage == voltage) {
        (*out) = (*low);
        return;
    }

    fix16_t t;
    if (low == high) {
        t = fix16_from_int(1);
    } else {
        t = fix16_sdiv(fix16_ssub(voltage, low->voltage), fix16_ssub(high->voltage, low->voltage));
    }
    uint32_t t_int = (uint32_t)(t);

    out->period_reg = _fix16_lerp_int(low->period_reg, high->period_reg, t_int << 16);
    out->castor_dac_code = fix16_lerp16(low->castor_dac_code, high->castor_dac_code, t_int);
    out->pollux_dac_code = fix16_lerp16(low->pollux_dac_code, high->pollux_dac_code, t_int);
    out->voltage = voltage;
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
    struct gem_voice_params* param_table,
    size_t table_len,
    fix16_t voltage,
    struct gem_voice_params** low,
    struct gem_voice_params** high) {
    (*low) = &param_table[0];
    (*high) = &param_table[0];
    bool found = false;
    for (size_t i = 0; i < table_len; i++) {
        struct gem_voice_params* current = &param_table[i];
        if (current->voltage <= voltage && current->voltage >= (*low)->voltage) {
            (*low) = current;
        }
        if (current->voltage > voltage) {
            (*high) = current;
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
    if (!found)
        (*high) = (*low);
};