#pragma once
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

void gem_adc_start();

uint16_t gem_adc_read_sync();

bool gem_adc_measurements_ready();

uint16_t* gem_adc_get_measurement_buffer();