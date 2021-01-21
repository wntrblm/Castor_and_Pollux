/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#ifdef DEBUG
#include "SEGGER_RTT.h"
#endif

void _putchar(char c) {
#ifdef DEBUG
    SEGGER_RTT_PutChar(0, c);
#else
    (void)(c);
#endif
}
