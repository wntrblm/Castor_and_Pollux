#include "gem_random.h"

uint16_t wyhash_state;
uint32_t xorshift32_state;
uint64_t xorshift64_state;

void gem_random_init(uint32_t seed) {
    wyhash_state = seed & 0xFFFF;
    xorshift32_state = seed;
    xorshift64_state = (uint64_t)(seed) << 32 | seed;
}

/* Modified wyhash algorithm, https://lemire.me/blog/2019/07/03/a-fast-16-bit-random-number-generator/ */
uint32_t _hash16(uint32_t input, uint32_t key) {
    uint32_t hash = input * key;
    return ((hash >> 16) ^ hash) & 0xFFFF;
}

uint16_t gem_random16() {
    wyhash_state += 0xfc15;
    return _hash16(wyhash_state, 0x2ab);
}

/* 32-bit xorshift, adapted from Wikipedia */
uint32_t gem_random32() {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x = xorshift32_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return xorshift32_state = x;
}

/* 64-bit xorshift, adapted from Wikipedia */
uint64_t gem_random64() {
    uint64_t x = xorshift64_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return xorshift64_state = x;
}