#pragma once
#include "fix16.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
    Gemini Analog to Digital Converter interface.
*/

struct gem_adc_input {
    uint8_t port;
    uint32_t pin;
    uint32_t ain;
};

void gem_adc_init(int16_t offset_error, uint16_t gain_error);

void gem_adc_init_input(const struct gem_adc_input* const input);

uint16_t gem_adc_read_sync(const struct gem_adc_input* input);

void gem_adc_start_scanning(const struct gem_adc_input* inputs, size_t num_inputs, uint32_t* results);

void gem_adc_stop_scanning();

bool gem_adc_results_ready();

struct gem_adc_errors {
    int16_t offset;
    fix16_t gain;
};

/* The SAMD series has harware correction, which we do use, however, since there are multiple types of
    inputs in play (unbuffered, buffered, some using potentiometers), these functions can apply further
    refinement after hardware adjustment.
*/

struct gem_adc_errors
gem_calculate_adc_errors(uint32_t low_measured, uint32_t low_expected, uint32_t high_measured, uint32_t high_expected);
uint16_t gem_correct_adc_errors(const uint16_t value, const struct gem_adc_errors errors);

#define GEM_CALCULATE_EXPECTED_ADC_CODE(value, range, resolution) \
    ((uint32_t)((value / range) * (resolution - 1)))
