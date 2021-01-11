/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "structy_config_.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Structy's core pack and unpack routines are based on Python's struct module.

Supported format characters:
* x: padding byte
* b: int8_t
* B: uint8_t
* h: int16_t
* H: uint16_t
* i: int32_t
* I: uint32_t
* ?: bool
* f: float

Caveats:
* Byte order is assumed to be network order (big endian).

*/

enum StructyResultStatus {
    /* All good. */
    STRUCTY_RESULT_OKAY,
    /* Structy tried to read or write past the the buffer. */
    STRUCTY_RESULT_BUF_OVERFLOW,
    /* Structy ran into an unknown format character. */
    STRUCTY_RESULT_UNKNOWN_FORMAT,
} STRUCTY_COMPILER_PACK;

struct StructyResult {
    enum StructyResultStatus status;

    /*
        How many items were packed/unpacked. If there was an error, this
        is the number of items that were processed before the error
        occurred.
    */
    size_t count;
};

size_t structy_size(const char* format);
struct StructyResult structy_unpack(const char* format, const uint8_t* buf, const size_t buf_len, ...);
struct StructyResult structy_pack(const char* format, uint8_t* buf, const size_t buf_len, ...);
