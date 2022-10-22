/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_math.h"
#include "gem_oscillator.h"
#include "gem_ramp_table.h"
#include <math.h>
#include <stdbool.h>

/*
    Uses the ramp look-up table to calculate the ramp control voltage for a
    given pitch control voltage.
*/

/* Forward declarations. */

static void find_nearest_pair_from_ramp_table_(
    fix16_t pitch_cv, struct GemRampTableEntry** low, struct GemRampTableEntry** high) RAMFUNC;

/* Public functions. */

uint32_t gem_ramp_table_lookup(uint8_t osc, fix16_t pitch_cv) {
    struct GemRampTableEntry* low = NULL;
    struct GemRampTableEntry* high = NULL;

    find_nearest_pair_from_ramp_table_(pitch_cv, &low, &high);

    uint16_t low_ramp_cv = 0;
    uint16_t high_ramp_cv = 0;

    if (osc == 0) {
        low_ramp_cv = low->castor_ramp_cv;
        high_ramp_cv = high->castor_ramp_cv;
    } else {
        low_ramp_cv = low->pollux_ramp_cv;
        high_ramp_cv = high->pollux_ramp_cv;
    }

    uint16_t lerp_amount = gem_f16_norm_dist_u16(low->pitch_cv, high->pitch_cv, pitch_cv);
    uint16_t lerped_cv = (uint16_t)gem_u32_lerp_u16(low_ramp_cv, high_ramp_cv, lerp_amount);
    return lerped_cv;
};

/* Private functions. */

static void
find_nearest_pair_from_ramp_table_(fix16_t pitch_cv, struct GemRampTableEntry** low, struct GemRampTableEntry** high) {
    (*low) = &gem_ramp_table[0];
    (*high) = &gem_ramp_table[0];
    struct GemRampTableEntry* current;
    bool found = false;

    for (size_t i = 0; i < gem_ramp_table_len; i++) {
        current = &gem_ramp_table[i];
        if (current->pitch_cv <= pitch_cv && current->pitch_cv >= (*low)->pitch_cv) {
            (*low) = current;
        }
        if (current->pitch_cv > pitch_cv) {
            (*high) = current;
            found = true;
            break;
        }
    }
    if (!found) {
        (*high) = (*low);
    }
}
