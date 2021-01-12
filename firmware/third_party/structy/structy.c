/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "structy.h"

size_t structy_size(const char* format) {
    size_t size = 0;
    size_t i = 0;
    while (format[i] != '\0') {
        switch (format[i]) {
            case 'x':
                size++;
                break;
            case 'b':
                size++;
                break;
            case 'B':
                size++;
                break;
            case '?':
                size++;
                break;
            case 'h':
                size += 2;
                break;
            case 'H':
                size += 2;
                break;
            case 'i':
                size += 4;
                break;
            case 'I':
                size += 4;
                break;
            case 'f':
                size += 4;
                break;
            default:
                break;
        }
        i++;
    }
    return size;
}

struct StructyResult structy_unpack(const char* format, const uint8_t* buf, const size_t buf_len, ...) {
    STRUCTY_ASSERT(buf != NULL);
    STRUCTY_ASSERT(buf_len >= structy_size(format));

    va_list args;
    va_start(args, buf_len);
    struct StructyResult result = {
        .count = 0,
        .status = STRUCTY_RESULT_OKAY,
    };

    size_t i = 0;
    size_t buf_idx = 0;
    while (format[i] != '\0') {
        if (buf_idx >= buf_len) {
            result.status = STRUCTY_RESULT_BUF_OVERFLOW;
            goto end;
        }

        switch (format[i]) {
            case 'x': {
                buf_idx++;
            } break;
            case 'b': {
                int8_t* var = va_arg(args, int8_t*);
                (*var) = (int8_t)(buf[buf_idx]);
                buf_idx++;
                result.count++;
            } break;
            case 'B': {
                uint8_t* var = va_arg(args, uint8_t*);
                (*var) = buf[buf_idx];
                buf_idx++;
                result.count++;
            } break;
            case '?': {
                bool* var = va_arg(args, bool*);
                (*var) = (bool)buf[buf_idx];
                buf_idx++;
                result.count++;
            } break;
            case 'h': {
                int16_t* var = va_arg(args, int16_t*);
                (*var) = buf[buf_idx] << 8u | buf[buf_idx + 1];
                buf_idx += 2;
                result.count++;
            } break;
            case 'H': {
                uint16_t* var = va_arg(args, uint16_t*);
                (*var) = buf[buf_idx] << 8u | buf[buf_idx + 1];
                buf_idx += 2;
                result.count++;
            } break;
            case 'i': {
                int32_t* var = va_arg(args, int32_t*);
                (*var) = buf[buf_idx] << 24u | buf[buf_idx + 1] << 16u | buf[buf_idx + 2] << 8u | buf[buf_idx + 3];
                buf_idx += 4;
                result.count++;
            } break;
            case 'I': {
                uint32_t* var = va_arg(args, uint32_t*);
                (*var) = (uint32_t)(
                    buf[buf_idx] << 24u | buf[buf_idx + 1] << 16u | buf[buf_idx + 2] << 8u | buf[buf_idx + 3]);
                buf_idx += 4;
                result.count++;
            } break;
#if STRUCTY_ENABLE_FLOAT == 1
            case 'f': {
                float* var = va_arg(args, float*);
                uint32_t temp = (uint32_t)(
                    buf[buf_idx] << 24u | buf[buf_idx + 1] << 16u | buf[buf_idx + 2] << 8u | buf[buf_idx + 3]);
                memcpy(var, &temp, 4);
                buf_idx += 4;
                result.count++;
            } break;
#endif
            default:
                result.status = STRUCTY_RESULT_UNKNOWN_FORMAT;
                goto end;
        }
        i++;
    }

end:
    va_end(args);
    return result;
}

#if STRUCTY_ENABLE_FLOAT == 1
union float_pun {
    uint32_t i;
    float f;
};
#endif

struct StructyResult structy_pack(const char* format, uint8_t* buf, const size_t buf_len, ...) {
    STRUCTY_ASSERT(buf != NULL);
    STRUCTY_ASSERT(buf_len >= structy_size(format));

    va_list args;
    va_start(args, buf_len);
    struct StructyResult result = {
        .count = 0,
        .status = STRUCTY_RESULT_OKAY,
    };

    size_t i = 0;
    size_t buf_idx = 0;
    while (format[i] != '\0') {
        if (buf_idx >= buf_len) {
            result.status = STRUCTY_RESULT_BUF_OVERFLOW;
            goto end;
        }

        switch (format[i]) {
            case 'x': {
                buf_idx++;
            } break;
            case 'b': {
                int8_t var = va_arg(args, int);
                buf[buf_idx] = *((uint8_t*)(&var));
                buf_idx++;
                result.count++;
            } break;
            case 'B': {
                uint8_t var = va_arg(args, int);
                buf[buf_idx] = var;
                buf_idx++;
                result.count++;
            } break;
            case '?': {
                bool var = va_arg(args, int);
                buf[buf_idx] = var;
                buf_idx++;
                result.count++;
            } break;
            case 'h': {
                int16_t var = va_arg(args, int);
                buf[buf_idx] = var >> 8u & 0xFF;
                buf[buf_idx + 1] = var & 0xFF;
                buf_idx += 2;
                result.count++;
            } break;
            case 'H': {
                uint16_t var = va_arg(args, int);
                buf[buf_idx] = var >> 8u & 0xFF;
                buf[buf_idx + 1] = var & 0xFF;
                buf_idx += 2;
                result.count++;
            } break;
            case 'i': {
                int32_t var = va_arg(args, int32_t);
                buf[buf_idx] = var >> 24u & 0xFF;
                buf[buf_idx + 1] = var >> 16u & 0xFF;
                buf[buf_idx + 2] = var >> 8u & 0xFF;
                buf[buf_idx + 3] = var & 0xFF;
                buf_idx += 4;
                result.count++;
            } break;
            case 'I': {
                uint32_t var = va_arg(args, uint32_t);
                buf[buf_idx] = var >> 24u & 0xFF;
                buf[buf_idx + 1] = var >> 16u & 0xFF;
                buf[buf_idx + 2] = var >> 8u & 0xFF;
                buf[buf_idx + 3] = var & 0xFF;
                buf_idx += 4;
                result.count++;
            } break;
#if STRUCTY_ENABLE_FLOAT == 1
            case 'f': {
                float var = va_arg(args, double);
                union float_pun temp;
                temp.f = var;
                buf[buf_idx] = temp.i >> 24u & 0xFF;
                buf[buf_idx + 1] = temp.i >> 16u & 0xFF;
                buf[buf_idx + 2] = temp.i >> 8u & 0xFF;
                buf[buf_idx + 3] = temp.i & 0xFF;
                buf_idx += 4;
                result.count++;
            } break;
#endif
            default:
                result.status = STRUCTY_RESULT_UNKNOWN_FORMAT;
                goto end;
        }
        i++;
    }

end:
    va_end(args);
    return result;
}
