#pragma once
#include <stdint.h>
#include <stdbool.h>

/*
    Gemini Analog to Digital Converter interface.
*/


void gem_adc_init();

void gem_adc_start();

uint16_t gem_adc_read_sync();

bool gem_adc_measurements_ready();

uint16_t* gem_adc_get_measurement_buffer();