/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Helpers for packing and unpacking data.

This doesn't do any byte-swapping or anything, see:
    https://commandcenter.blogspot.com/2012/04/byte-order-fallacy.html
*/

#define UNPACK_16(data, idx) data[idx] << 8 | data[idx + 1]
#define UNPACK_32(data, idx) data[idx] << 24 | data[idx + 1] << 16 | data[idx + 2] << 8 | data[idx + 3]

#define PACK_16(val, data, idx)                                                                                        \
    data[idx] = val >> 8;                                                                                              \
    data[idx + 1] = val & 0xFF;
#define PACK_32(val, data, idx)                                                                                        \
    data[idx] = val >> 24;                                                                                             \
    data[idx + 1] = val >> 16;                                                                                         \
    data[idx + 2] = val >> 8;                                                                                          \
    data[idx + 3] = val & 0xFF;
