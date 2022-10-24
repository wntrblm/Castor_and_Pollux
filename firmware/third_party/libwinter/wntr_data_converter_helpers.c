/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_data_converter_helpers.h"
#include "printf.h"

void WntrVoltageCalibrationTable_find_nearest_pair(
    float value,
    struct WntrVoltageCalibrationTable table,
    struct WntrVoltageCalibrationTableEntry* out_low,
    struct WntrVoltageCalibrationTableEntry* out_high) {

    bool found = false;
    struct WntrVoltageCalibrationTableEntry* low = &(table.entries)[0];
    struct WntrVoltageCalibrationTableEntry* high = &(table.entries)[1];
    struct WntrVoltageCalibrationTableEntry* current;

    for (size_t i = 0; i < table.len; i++) {
        current = &(table.entries)[i];
        if (current->measured <= value) {
            low = current;
        }
        if (current->measured > value) {
            high = current;
            found = true;
            break;
        }
    }

    if (!found) {
        high = low;
    }

    (*out_low) = (*low);
    (*out_high) = (*high);
}

float WntrVoltageCalibrationTable_lookup(float value, struct WntrVoltageCalibrationTable table) {
    struct WntrVoltageCalibrationTableEntry low, high;

    WntrVoltageCalibrationTable_find_nearest_pair(value, table, &low, &high);

    if (low.expected == high.expected) {
        return high.expected;
    }

    float frac = (value - low.measured) / (high.measured - low.measured);

    float result = low.expected + ((high.expected - low.expected) * frac);

    return result;
}

void WntrVoltageCalibrationTable_print(struct WntrVoltageCalibrationTable table) {
    printf("---- WntrVoltageCalibrationTable 0x%zx + %zu\n", (size_t)(table.entries), table.len);
    for (size_t i = 0; i < table.len; i++) {
        printf("%0.3f : %0.3f\n", (double)(table.entries[i].expected), (double)(table.entries[i].measured));
    }
    printf("----\n");
}
