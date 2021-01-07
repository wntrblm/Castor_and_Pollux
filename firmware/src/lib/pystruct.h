#pragma once

/* A half-baked port of Python's struct module.

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

#include <stdint.h>
#include <stddef.h>

enum pystruct_result {
    PYSTRUCT_RESULT_OKAY,
    PYSTRUCT_RESULT_BUF_OVERFLOW,
    PYSTRUCT_RESULT_UNKNOWN_FORMAT,
};

enum pystruct_result pystruct_unpack(const char* format, const uint8_t* buf, const size_t buf_len, ...);
enum pystruct_result pystruct_pack(const char* format, uint8_t* buf, const size_t buf_len, ...);