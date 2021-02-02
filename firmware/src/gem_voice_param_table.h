/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for managing the global look-up table for mapping CV inputs to
    appropriate timer periods and DAC codes to generate the correct note.
*/

#include "gem_voice_params.h"

extern const struct GemVoltageAndPeriod gem_voice_voltage_and_period_table[];
extern struct GemDACCodePair gem_voice_dac_codes_table[];
extern size_t gem_voice_param_table_len;

void gem_load_dac_codes_table();
void gem_save_dac_codes_table();
void gem_erase_dac_codes_table();
