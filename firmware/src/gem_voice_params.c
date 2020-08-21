#include <math.h>
#include <stdbool.h>

#include "gem_lerp.h"
#include "gem_voice_params.h"

/* Private function forward declarations. */
static void _find_nearest_table_entries(struct gem_voice_params* param_table,
                                        size_t table_len,
                                        uint16_t adc_code,
                                        struct gem_voice_params** low,
                                        struct gem_voice_params** high);

/* Public functions. */

void gem_voice_params_from_adc_code(struct gem_voice_params* table,
                                    size_t table_len,
                                    uint16_t adc_code,
                                    struct gem_voice_params* out) {
    struct gem_voice_params* low;
    struct gem_voice_params* high;

    _find_nearest_table_entries(table, table_len, adc_code, &low, &high);

    float t = (float)(adc_code - low->adc_code) / (float)(high->adc_code - low->adc_code);
    out->period_reg = (uint16_t)roundf(lerpf(low->period_reg, high->period_reg, t));
    out->dac_code = (uint16_t)roundf(lerpf(low->dac_code, high->dac_code, t));
    out->adc_code = adc_code;
};

/* Private functions. */

/* Used to find the two nearest table entries for doing linear interpolation.
   the table entries are defined in gem_voice_params_table.h.
*/
static void _find_nearest_table_entries(struct gem_voice_params* param_table,
                                        size_t table_len,
                                        uint16_t adc_code,
                                        struct gem_voice_params** low,
                                        struct gem_voice_params** high) {
    (*low) = &param_table[0];
    (*high) = &param_table[0];
    bool found = false;
    for (size_t i = 0; i < table_len; i++) {
        struct gem_voice_params* current = &param_table[i];
        if (current->adc_code <= adc_code && current->adc_code >= (*low)->adc_code) {
            (*low) = current;
        }
        if (current->adc_code > adc_code) {
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