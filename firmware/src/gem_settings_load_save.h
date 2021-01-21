/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for managing NVM-backed user-changeable settings. */

#include "fix16.h"
#include "gem_settings.h"
#include <stdbool.h>
#include <stdint.h>

bool GemSettings_load(struct GemSettings* settings);
void GemSettings_save(struct GemSettings* settings);
void GemSettings_erase();
