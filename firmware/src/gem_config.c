#include "gem_config.h"

const struct GemADCInput gem_adc_inputs[] = {
    {0, 6, ADC_INPUTCTRL_MUXPOS_PIN6},   // CV A
    {1, 3, ADC_INPUTCTRL_MUXPOS_PIN11},  // CV A Pot
    {0, 7, ADC_INPUTCTRL_MUXPOS_PIN7},   // CV B
    {1, 2, ADC_INPUTCTRL_MUXPOS_PIN10},  // CV B Pot
    {0, 5, ADC_INPUTCTRL_MUXPOS_PIN5},   // Duty A
    {0, 8, ADC_INPUTCTRL_MUXPOS_PIN16},  // Duty A Pot
    {1, 9, ADC_INPUTCTRL_MUXPOS_PIN3},   // Duty B
    {0, 9, ADC_INPUTCTRL_MUXPOS_PIN17},  // Duty B Pot
    {0, 2, ADC_INPUTCTRL_MUXPOS_PIN0},   // Chorus pot
};
