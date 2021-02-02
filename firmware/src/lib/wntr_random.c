/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_random.h"

static uint16_t wyhash_state;
static uint32_t xorshift32_state;
static uint64_t xorshift64_state;

void wntr_random_init(uint32_t seed) {
    wyhash_state = seed & 0xFFFF;
    xorshift32_state = seed;
    xorshift64_state = (uint64_t)(seed) << 32 | seed;
}

/* Modified wyhash algorithm, https://lemire.me/blog/2019/07/03/a-fast-16-bit-random-number-generator/ */
static uint32_t hash16_(uint32_t input, uint32_t key) {
    uint32_t hash = input * key;
    return ((hash >> 16) ^ hash) & 0xFFFF;
}

uint16_t wntr_random16() {
    wyhash_state += 0xfc15;
    return hash16_(wyhash_state, 0x2ab);
}

/* 32-bit xorshift, adapted from Wikipedia */
uint32_t wntr_random32() {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x = xorshift32_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return xorshift32_state = x;
}

/* 64-bit xorshift, adapted from Wikipedia */
uint64_t wntr_random64() {
    uint64_t x = xorshift64_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return xorshift64_state = x;
}
