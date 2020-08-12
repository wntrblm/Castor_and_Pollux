#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
    Gemini Analog to Digital Converter interface.
*/

struct gem_adc_input {
    uint8_t port;
    uint32_t pin;
    uint32_t ain;
};

void gem_adc_init();

void gem_adc_init_input(const struct gem_adc_input* const input);

uint16_t gem_adc_read_sync(const struct gem_adc_input* input);

void gem_adc_start_scanning(const struct gem_adc_input* inputs, size_t num_inputs, uint32_t* results);

bool gem_adc_results_ready();