/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for managing Gemini's ramp look-up table.

    The ramp table is used to determine the charge voltage to send into the
    ramp core to compensation for amplitude loss as frequency increases. This
    table is written to NVM during calibration.
*/

#include "fix16.h"
#include "wntr_ramfunc.h"
#include <stddef.h>
#include <stdint.h>

struct GemRampTableEntry {
    fix16_t pitch_cv;
    uint16_t castor_ramp_cv;
    uint16_t pollux_ramp_cv;
};

extern struct GemRampTableEntry gem_ramp_table[];
extern size_t gem_ramp_table_len;

void gem_ramp_table_load();
void gem_ramp_table_save();
void gem_ramp_table_erase();
uint32_t gem_ramp_table_lookup(uint8_t osc, fix16_t pitch_cv) RAMFUNC;
