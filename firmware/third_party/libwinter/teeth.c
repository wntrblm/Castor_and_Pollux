/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "teeth.h"

void teeth_encode(const uint8_t* src, size_t src_len, uint8_t* dst) {
    size_t src_idx = 0;
    size_t dst_idx = 0;

    while (src_idx < src_len) {
        // There's 4 or more bytes left
        if (src_idx + 4 <= src_len) {
            // First byte carries the leftover bits.
            dst[dst_idx] = 0x40 | ((src[src_idx] & 0x80) >> 4) | ((src[src_idx + 1] & 0x80) >> 5) |
                           ((src[src_idx + 2] & 0x80) >> 6) | ((src[src_idx + 3] & 0x80) >> 7);
            // Subsequent bytes carry their lower 7 bits.
            dst[dst_idx + 1] = src[src_idx] & 0x7F;
            dst[dst_idx + 2] = src[src_idx + 1] & 0x7F;
            dst[dst_idx + 3] = src[src_idx + 2] & 0x7F;
            dst[dst_idx + 4] = src[src_idx + 3] & 0x7F;
            dst_idx += 5;
            src_idx += 4;
        }
        // There's only 3 bytes left
        else if (src_idx + 3 == src_len) {
            dst[dst_idx] = 0x30 | ((src[src_idx] & 0x80) >> 4) | ((src[src_idx + 1] & 0x80) >> 5) |
                           ((src[src_idx + 2] & 0x80) >> 6);
            dst[dst_idx + 1] = src[src_idx] & 0x7F;
            dst[dst_idx + 2] = src[src_idx + 1] & 0x7F;
            dst[dst_idx + 3] = src[src_idx + 2] & 0x7F;
            dst_idx += 4;
            src_idx += 3;
        }
        // There's only 2 bytes left
        else if (src_idx + 2 == src_len) {
            dst[dst_idx] = 0x20 | ((src[src_idx] & 0x80) >> 4) | ((src[src_idx + 1] & 0x80) >> 5);
            dst[dst_idx + 1] = src[src_idx] & 0x7F;
            dst[dst_idx + 2] = src[src_idx + 1] & 0x7F;
            dst_idx += 3;
            src_idx += 2;
        }
        // There's only 1 byte left
        else if (src_idx + 1 == src_len) {
            dst[dst_idx] = 0x10 | ((src[src_idx] & 0x80) >> 4);
            dst[dst_idx + 1] = src[src_idx] & 0x7F;
            dst_idx += 2;
            src_idx += 1;
        } else {
            break;
        }
    }
}

size_t teeth_decode(const uint8_t* src, size_t src_len, uint8_t* dst) {
    // assert(src_len % 5 == 0);
    size_t src_idx = 0;
    size_t dst_idx = 0;

    while (src_idx < src_len) {
        dst[dst_idx] = (src[src_idx] & 0x8) << 4 | src[src_idx + 1];
        dst[dst_idx + 1] = (src[src_idx] & 0x4) << 5 | src[src_idx + 2];
        dst[dst_idx + 2] = (src[src_idx] & 0x2) << 6 | src[src_idx + 3];
        dst[dst_idx + 3] = (src[src_idx] & 0x1) << 7 | src[src_idx + 4];
        uint8_t len_marker = src[src_idx] >> 4;
        dst_idx += len_marker;
        src_idx += 5;
    }

    return dst_idx;
}

#ifdef TEETH_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]) {
    printf("Teeth encode/decode tests\n");

    srand(time(NULL));

    uint8_t source_buf[128];
    uint8_t dest_buf[TEETH_ENCODED_LENGTH(128)];
    uint8_t result_buf[128];

    printf("Random data, fixed length buffers...");
    for (size_t n = 0; n < 100; n++) {
        for (size_t i = 0; i < 128; i++) { source_buf[i] = (uint8_t)rand(); }

        teeth_encode(source_buf, 128, dest_buf);
        teeth_decode(dest_buf, TEETH_ENCODED_LENGTH(128), result_buf);

        assert(memcmp(source_buf, result_buf, 128) == 0);
    }
    printf("ok!\n");

    printf("Random data, random length buffers...");
    for (size_t n = 0; n < 100; n++) {
        size_t len = rand() % 128;
        for (size_t i = 0; i < len; i++) { source_buf[i] = (uint8_t)rand(); }

        teeth_encode(source_buf, len, dest_buf);
        teeth_decode(dest_buf, TEETH_ENCODED_LENGTH(len), result_buf);

        assert(memcmp(source_buf, result_buf, len) == 0);
    }

    printf("ok!\n");

    printf("All done :)\n");
}

#endif
