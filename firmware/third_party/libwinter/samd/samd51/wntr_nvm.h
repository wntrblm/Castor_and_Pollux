/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include <stddef.h>
#include <stdint.h>

void wntr_nvm_write(uint8_t* dst, uint8_t* src, size_t src_len);
