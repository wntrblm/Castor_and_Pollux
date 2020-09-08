#include <math.h>
#include <stdbool.h>

#include "gem_lerp.h"
#include "gem_voice_params.h"

/* Private function forward declarations. */
static void _find_nearest_table_entries(struct gem_voice_params* param_table,
                                        size_t table_len,
                                        float voltage,
                                        struct gem_voice_params** low,
                                        struct gem_voice_params** high);

/* Public functions. */

void gem_voice_params_from_cv(struct gem_voice_params* table,
                              size_t table_len,
                              float voltage,
                              struct gem_voice_params* out) {
    struct gem_voice_params* low;
    struct gem_voice_params* high;

    _find_nearest_table_entries(table, table_len, voltage, &low, &high);

    float t = (float)(voltage - low->voltage) / (float)(high->voltage - low->voltage);
    out->period_reg = (uint16_t)roundf(lerpf(low->period_reg, high->period_reg, t));
    out->castor_dac_code = (uint16_t)roundf(lerpf(low->castor_dac_code, high->castor_dac_code, t));
    out->pollux_dac_code = (uint16_t)roundf(lerpf(low->pollux_dac_code, high->pollux_dac_code, t));
    out->voltage = voltage;
};

/* Private functions. */

/* Used to find the two nearest table entries for doing linear interpolation.
   the table entries are defined in gem_voice_params_table.h.
*/
static void _find_nearest_table_entries(struct gem_voice_params* param_table,
                                        size_t table_len,
                                        float voltage,
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