#pragma once

#include <stddef.h>
#include <stdint.h>

void gem_nvm_init();

void gem_nvm_read(uint32_t src, uint8_t* buf, size_t len);

void gem_nvm_write(uint32_t dst, const uint8_t* buf, size_t len);