/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "printf.h"

inline static void wntr_debug_print_mem(uint8_t* addr, size_t len) {
    printf("---- MEMORY DUMP 0x%x + %u\n", addr, len);
    for (size_t i = 0; i < len; i += 16) {
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            printf("%02x ", addr[i + j]);
        }
        printf("\n");
    }
    printf("---- END MEMORY DUMP\n");
}
