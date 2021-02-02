/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Light-weight asserts used across wntr/gem code.

    These asserts are active during release builds, unless WNTR_ASSERT_DEBUG
    is used.
*/

/* Include assert for static_assert */
#include <assert.h>

void _wntr_assert(const char* file, int line) __attribute__((__noreturn__));

#define WNTR_ASSERT(expr) ((expr) ? (void)0 : _wntr_assert(__BASE_FILE__, __LINE__))

#ifdef NDEBUG
#define WNTR_ASSERT_DEBUG(expr) ((void)0)
#else
#define WNTR_ASSERT_DEBUG(expr) WNTR_ASSERT(expr)
#endif
