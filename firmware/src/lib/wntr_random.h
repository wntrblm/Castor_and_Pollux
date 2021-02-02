/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for pseudorandom number generation. */

#include <stdint.h>

void wntr_random_init(uint32_t seed);

uint16_t wntr_random16();
uint32_t wntr_random32();
uint64_t wntr_random64();
