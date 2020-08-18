#include "gem_config.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#endif

const struct gem_adc_input gem_adc_inputs[] = {
    {1, PORT_PB09, ADC_INPUTCTRL_MUXPOS_PIN3},
    {1, PORT_PB08, ADC_INPUTCTRL_MUXPOS_PIN2},
};

void gem_config_init() {}

int __wrap_printf(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    int result = SEGGER_RTT_vprintf(0, format, &args);
    va_end(args);
    return result;
#else
    (void)format;
    return 0;
#endif
}