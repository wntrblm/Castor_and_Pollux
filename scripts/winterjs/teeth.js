/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

export function encoded_length(src_len) {
    if (src_len instanceof Uint8Array) {
        src_len = src_len.length;
    }
    return (Math.floor((src_len + 4 - 1) / 4)) * 5;
}

export function encode(src) {
    const src_len = src.length;
    let dst = new Uint8Array(encoded_length(src_len));
    let src_idx = 0;
    let dst_idx = 0;

    while (src_idx < src_len) {
        // There's 4 or more bytes left
        if (src_idx + 4 <= src_len) {
            // First byte carries the leftover bits.
            dst[dst_idx] =
                0x40 |
                ((src[src_idx] & 0x80) >> 4) |
                ((src[src_idx + 1] & 0x80) >> 5) |
                ((src[src_idx + 2] & 0x80) >> 6) |
                ((src[src_idx + 3] & 0x80) >> 7);
            // Subsequent bytes carry their lower 7 bits.
            dst[dst_idx + 1] = src[src_idx] & 0x7f;
            dst[dst_idx + 2] = src[src_idx + 1] & 0x7f;
            dst[dst_idx + 3] = src[src_idx + 2] & 0x7f;
            dst[dst_idx + 4] = src[src_idx + 3] & 0x7f;
            dst_idx += 5;
            src_idx += 4;
        }
        // There's only 3 bytes left
        else if (src_idx + 3 == src_len) {
            dst[dst_idx] =
                0x30 |
                ((src[src_idx] & 0x80) >> 4) |
                ((src[src_idx + 1] & 0x80) >> 5) |
                ((src[src_idx + 2] & 0x80) >> 6);
            dst[dst_idx + 1] = src[src_idx] & 0x7f;
            dst[dst_idx + 2] = src[src_idx + 1] & 0x7f;
            dst[dst_idx + 3] = src[src_idx + 2] & 0x7f;
            dst_idx += 4;
            src_idx += 3;
        }
        // There's only 2 bytes left
        else if (src_idx + 2 == src_len) {
            dst[dst_idx] =
                0x20 |
                ((src[src_idx] & 0x80) >> 4) |
                ((src[src_idx + 1] & 0x80) >> 5);
            dst[dst_idx + 1] = src[src_idx] & 0x7f;
            dst[dst_idx + 2] = src[src_idx + 1] & 0x7f;
            dst_idx += 3;
            src_idx += 2;
        }
        // There's only 1 byte left
        else if (src_idx + 1 == src_len) {
            dst[dst_idx] = 0x10 | ((src[src_idx] & 0x80) >> 4);
            dst[dst_idx + 1] = src[src_idx] & 0x7f;
            dst_idx += 2;
            src_idx += 1;
        } else {
            break;
        }
    }

    return dst;
}

export function decode(src) {
    const src_len = src.length;
    const dst_len = (src_len / 5) * 4;
    let dst = new Uint8Array(dst_len);
    let src_idx = 0;
    let dst_idx = 0;

    while (src_idx < src_len) {
        dst[dst_idx] = ((src[src_idx] & 0x8) << 4) | src[src_idx + 1];
        dst[dst_idx + 1] = ((src[src_idx] & 0x4) << 5) | src[src_idx + 2];
        dst[dst_idx + 2] = ((src[src_idx] & 0x2) << 6) | src[src_idx + 3];
        dst[dst_idx + 3] = ((src[src_idx] & 0x1) << 7) | src[src_idx + 4];
        let len_marker = src[src_idx] >> 4;
        dst_idx += len_marker;
        src_idx += 5;
    }

    return dst.slice(0, dst_idx);
}
