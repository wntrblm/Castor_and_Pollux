/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for pseudo-random number generation. */

#include <stdint.h>

void gem_random_init(uint32_t seed);
uint16_t gem_random16();
uint32_t gem_random32();
uint64_t gem_random64();
