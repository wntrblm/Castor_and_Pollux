#pragma once

#include <stddef.h>
#include <stdint.h>

void gem_dotstar_init(uint8_t brightness);
void gem_dotstar_set(size_t n, uint8_t r, uint8_t g, uint8_t b);
void gem_dotstar_update();