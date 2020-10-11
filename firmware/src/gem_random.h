#pragma once

#include <stdint.h>

void gem_random_init(uint32_t seed);
uint16_t gem_random16();
uint32_t gem_random32();
uint64_t gem_random64();