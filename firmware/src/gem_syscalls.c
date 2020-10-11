#include "SEGGER_RTT.h"

void _putchar(char c) {
#ifdef DEBUG
    SEGGER_RTT_PutChar(0, c);
#endif
}