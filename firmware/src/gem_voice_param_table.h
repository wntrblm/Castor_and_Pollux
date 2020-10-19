#pragma once

#include "gem_voice_params.h"

extern const struct gem_voice_voltage_and_period gem_voice_voltage_and_period_table[];
extern struct gem_voice_dac_codes gem_voice_dac_codes_table[];
extern size_t gem_voice_param_table_len;

void gem_load_dac_codes_table();
void gem_save_dac_codes_table();