/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Teeth encoding/decoding routines

> "What the heck is Teeth encoding?"

It's a really dumb encoding scheme designed to deal with sending data over
MIDI SysEx which is limited to 7-bit values. It encodes each 32-bit (4 byte)
value using 5 bytes, therefore, it has a 25% overhead. It's designed to
be deterministic so that C implementations can avoid dynamic allocation -
regardless of the number of items in the encoder input, the encoded output
is *always* a deterministic multiple of 5.

The basic scheme is that the following 32-bit value:

0x1AAA AAAA
0x2BBB BBBB
0x3CCC CCCC
0x4DDD DDDD

Gets encoded as:

0x4A <--- header byte
0x0AAA AAAA
0x0BBB BBBB
0x0CCC CCCC
0x0DDD DDDD

The header byte's upper nibble is number of following bytes, its max value is
4. This allows encoding and decoding arrays that aren't a multiple of 4 bytes
in length and preserving the length during decoding.

The second nibble contains the most significant bit for each of the
directly following bytes as 0b1234.

> "Why the HECK did you name it Teeth?"

It works on 32-bit values and humans happen to have 32 teeth, also, it takes
one more byte than usual to encode so it's an *overbyte*...

*/

#include <stddef.h>
#include <stdint.h>

#define TEETH_ENCODED_LENGTH(src_len) (((src_len + 4 - 1) / 4) * 5)
#define TEETH_DECODED_LENGTH(src_len) (src_len / 5 * 4)

void teeth_encode(const uint8_t* src, size_t src_len, uint8_t* dst);
size_t teeth_decode(const uint8_t* src, size_t src_len, uint8_t* dst);
