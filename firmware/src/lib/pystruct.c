#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include "pystruct.h"


enum pystruct_result pystruct_unpack(const char* format, const uint8_t* buf, const size_t buf_len, ...) {
    va_list args;
    va_start(args, buf_len);
    enum pystruct_result result = PYSTRUCT_RESULT_OKAY;

    size_t i = 0;
    size_t buf_idx = 0;
    while (format[i] != '\0') {
        if(buf_idx >= buf_len) {
            result = PYSTRUCT_RESULT_BUF_OVERFLOW;
            goto end;
        }

        switch(format[i]) {
            case 'x': {
                    buf_idx++;
                }
                break;
            case 'b': {
                    int8_t* var = va_arg(args, int8_t*);
                    (*var) = (int8_t)(buf[buf_idx]);
                    buf_idx++;
                }
                break;
            case 'B': {
                    uint8_t* var = va_arg(args, uint8_t*);
                    (*var) = buf[buf_idx];
                    buf_idx++;
                }
                break;
            case '?': {
                    bool* var = va_arg(args, bool*);
                    (*var) = (bool)buf[buf_idx];
                    buf_idx++;
                }
                break;
            case 'h': {
                    int16_t* var = va_arg(args, int16_t*);
                    (*var) = buf[buf_idx] << 8 | buf[buf_idx+1];
                    buf_idx+=2;
                }
                break;
            case 'H': {
                    uint16_t* var = va_arg(args, uint16_t*);
                    (*var) = buf[buf_idx] << 8 | buf[buf_idx+1];
                    buf_idx+=2;
                }
                break;
            case 'i': {
                    int32_t* var = va_arg(args, int32_t*);
                    (*var) = buf[buf_idx] << 24 | buf[buf_idx+1] << 16 | buf[buf_idx+2] << 8 | buf[buf_idx+3];
                    buf_idx+=4;
                }
                break;
            case 'I': {
                    uint32_t* var = va_arg(args, uint32_t*);
                    (*var) = buf[buf_idx] << 24 | buf[buf_idx+1] << 16 | buf[buf_idx+2] << 8 | buf[buf_idx+3];
                    buf_idx+=4;
                }
                break;
            case 'f': {
                    float* var = va_arg(args, float*);
                    uint32_t temp = buf[buf_idx] << 24 | buf[buf_idx+1] << 16 | buf[buf_idx+2] << 8 | buf[buf_idx+3];
                    memcpy(var, &temp, 4);
                    buf_idx+=4;
                }
                break;
            default:
                result = PYSTRUCT_RESULT_UNKNOWN_FORMAT;
                goto end;
        }
        i++;
    }

end:
    va_end(args);
    return result;
}


enum pystruct_result pystruct_pack(const char* format, uint8_t* buf, const size_t buf_len, ...) {
    va_list args;
    va_start(args, buf_len);
    enum pystruct_result result = PYSTRUCT_RESULT_OKAY;

    size_t i = 0;
    size_t buf_idx = 0;
    while (format[i] != '\0') {
        if(buf_idx >= buf_len) {
            result = PYSTRUCT_RESULT_BUF_OVERFLOW;
            goto end;
        }

        switch(format[i]) {
            case 'x': {
                    buf_idx++;
                }
                break;
            case 'b': {
                    int8_t var = va_arg(args, int);
                    buf[buf_idx] = *((uint8_t*)(&var));
                    buf_idx++;
                }
                break;
            case 'B': {
                    uint8_t var = va_arg(args, int);
                    buf[buf_idx] = var;
                    buf_idx++;
                }
                break;
            case '?': {
                    bool var = va_arg(args, int);
                    buf[buf_idx] = var;
                    buf_idx++;
                }
                break;
            case 'h': {
                    int16_t var = va_arg(args, int);
                    buf[buf_idx] = var >> 8 & 0xFF;
                    buf[buf_idx+1] = var & 0xFF;
                    buf_idx+=2;
                }
                break;
            case 'H': {
                    uint16_t var = va_arg(args, int);
                    buf[buf_idx] = var >> 8 & 0xFF;
                    buf[buf_idx+1] = var & 0xFF;
                    buf_idx+=2;
                }
                break;
            case 'i': {
                    int32_t var = va_arg(args, int32_t);
                    buf[buf_idx] = var >> 24 & 0xFF;
                    buf[buf_idx+1] = var >> 16 & 0xFF;
                    buf[buf_idx+2] = var >> 8 & 0xFF;
                    buf[buf_idx+3] = var & 0xFF;
                    buf_idx+=4;
                }
                break;
            case 'I': {
                    uint32_t var = va_arg(args, uint32_t);
                    buf[buf_idx] = var >> 24 & 0xFF;
                    buf[buf_idx+1] = var >> 16 & 0xFF;
                    buf[buf_idx+2] = var >> 8 & 0xFF;
                    buf[buf_idx+3] = var & 0xFF;
                    buf_idx+=4;
                }
                break;
            case 'f': {
                    float var = va_arg(args, double);
                    uint32_t temp = *((uint32_t*)(&var));
                    buf[buf_idx] = temp >> 24 & 0xFF;
                    buf[buf_idx+1] = temp >> 16 & 0xFF;
                    buf[buf_idx+2] = temp >> 8 & 0xFF;
                    buf[buf_idx+3] = temp & 0xFF;
                    buf_idx+=4;
                }
                break;
            default:
                result = PYSTRUCT_RESULT_UNKNOWN_FORMAT;
                goto end;
        }
        i++;
    }


end:
    va_end(args);
    return result;
}


#ifdef PYSTRUCT_TEST
#include <assert.h>

int main() {
    // clang-format off
    const uint8_t buf[] = {
        0x7F,
        -32,
        /* 43690 */0xAA, 0xAA,
        /* -23211 */0xA5, 0x55,
        /* 3132799674 */0xBA, 0xBA, 0xBA, 0xBA,
        /* -1094795586 */0xBE, 0xBE, 0xBE, 0xBE,
        /* 3.145 */0x40, 0x49, 0x47, 0xAE,
        0x01,
    };

    uint8_t uint8;
    int8_t sint8;
    uint16_t uint16;
    int16_t sint16;
    uint32_t uint32;
    int32_t sint32;
    float floatv;
    bool boolv;

    printf("Testing unpack...");
    
    pystruct_unpack("BbHhIif?", buf, 19, &uint8, &sint8, &uint16, &sint16, &uint32, &sint32, &floatv, &boolv);

    assert(uint8 == 127);
    assert(sint8 == -32);
    assert(uint16 == 43690);
    assert(sint16 == -23211);
    assert(uint32 == 3132799674);
    assert(sint32 == -1094795586);
    assert(floatv = 3.145f);
    assert(boolv == true);

    printf("okay!\n");

    printf("Testing pack...");

    uint8_t pbuf[19];

    pystruct_pack("BbHhIif?", pbuf, 19, uint8, sint8, uint16, sint16, uint32, sint32, floatv, boolv);

    assert(memcmp(pbuf, buf, 19) == 0);

    printf("okay!\n");

    printf("All done! :)\n");

    return 0;
    // clang-format on
}
#endif