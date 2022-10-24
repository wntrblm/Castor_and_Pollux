/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Helper macros for dealing with arrays, notably WNTR_ARRAY_LEN
*/

// Largely based on:
// - http://zubplot.blogspot.com/2015/01/gcc-is-wonderful-better-arraysize-macro.html
// - https://stackoverflow.com/questions/12849714/is-there-a-type-safe-way-of-getting-an-element-count-for-arrays-in-c
#define _WNTR_BUILD_BUG_ON_ZERO(e) \
    (sizeof(struct { int:-!!(e)*1234; }))
#define _WNTR_SAME_TYPE(a, b) \
     __builtin_types_compatible_p(typeof(a), typeof(b))
#define WNTR_MUST_BE_ARRAY(a) \
     _WNTR_BUILD_BUG_ON_ZERO(_WNTR_SAME_TYPE((a), &(*a)))
#define WNTR_ARRAY_LEN(a) ( \
    (sizeof(a) / sizeof(*a)) \
    + WNTR_MUST_BE_ARRAY(a))
