/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include "gem_monitor_update.h"
#include <stdbool.h>
#include <stdint.h>

/* Implementations of SysEx commands for calibration, setup, and configuration. */

void gem_register_sysex_commands();

void gem_sysex_send_monitor_update(struct GemMonitorUpdate* update);
