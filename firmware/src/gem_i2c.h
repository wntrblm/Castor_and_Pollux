#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "sam.h"


void gem_i2c_init();

bool gem_i2c_write(uint8_t address, uint8_t* data, size_t len);