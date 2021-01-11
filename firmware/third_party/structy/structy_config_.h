/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Global configuration for Structy. To modify these values,
    create a "structy_config.h" in your application.

    "struct_config.h" should include any needed headers expanded
    macros.
*/

#if __has_include("structy_config.h")
#include "structy_config.h"
#endif

/* STRUCTY_PRINTF is used to print struct values and debug messages. */
#ifndef STRUCTY_PRINTF
#ifdef __arm__
/* Check for the nice embedded printf library */
#if __has_include("printf.h")
#include "printf.h"
#define STRUCTY_PRINTF(...) printf(__VA_ARGS__)
#else
#define STRUCTY_PRINTF(...)
#endif
#else
/* For big-girl computers just use stdio and stderr. */
#include <stdio.h>
#define STRUCTY_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#endif
#endif

/* STRUCTY_ASSERT is used to make assertions when debug is active. */
#ifndef STRUCTY_ASSERT
#if defined(DEBUG) && !defined(NDEBUG) && !defined(__arm__)
#include <assert.h>
#define STRUCTY_ASSERT(...) assert(__VA_ARGS__)
#else
#define STRUCTY_ASSERT(...)
#endif
#endif

/* STRUCTY_COMPILER_PACK is used to apply packed attributes to enums/structs. */
#ifndef STRUCTY_COMPILER_PACK
#if defined(__gcc__) || defined(__clang__)
#define STRUCTY_COMPILER_PACK __attribute__((__packed__))
#else
#define STRUCTY_COMPILER_PACK
#endif
#endif
