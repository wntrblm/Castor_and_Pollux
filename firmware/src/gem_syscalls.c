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