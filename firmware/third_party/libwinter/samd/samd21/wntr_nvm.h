/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for reading and writing flash memory (NVM) on the SAM D21. */

#include <stddef.h>
#include <stdint.h>

void wntr_nvm_read(uint32_t src, uint8_t* buf, size_t len);

void wntr_nvm_write(uint32_t dst, const uint8_t* buf, size_t len);
