/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for interacting with SPI devices. */

#include <stddef.h>
#include <stdint.h>

void gem_spi_init();

void gem_spi_write(const uint8_t* data, size_t len);
