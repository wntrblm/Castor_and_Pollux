#pragma once

#include <stddef.h>
#include <stdint.h>

void gem_spi_init();

void gem_spi_write(uint8_t* data, size_t len);