#pragma once

/* Routines for interacting with the SAM D21 analog-to-digital converter. */

#include "fix16.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


/*
    Input channel descriptor. These settings should be derived from the
    pin multiplexing table in the datasheet.
*/
struct GemADCInput {
    /* IO port for the input */
    uint8_t port;
    /* Pin number for the input */
    uint32_t pin;
    /* AIN number for the input */
    uint32_t ain;
};

void gem_adc_init(int16_t offset_error, uint16_t gain_error);

/* Configure hardware error correction. */
void gem_adc_set_error_correction(uint16_t gain, uint16_t offset);

/* Configure a given input channel for ADC reading. */
void gem_adc_init_input(const struct GemADCInput* const input);

uint16_t gem_adc_read_sync(const struct GemADCInput* input);

/*
    Start scanning input channels using the ADC's result ready interrupt.
    The results array will be continously updated with new ADC readings.
    You can check when a complete set of readings is ready by calling
    gem_adc_results_ready().
*/
void gem_adc_start_scanning(const struct GemADCInput* inputs, size_t num_inputs, uint32_t* results);

void gem_adc_stop_scanning();

/* Check if the channel scanning has finished scanning all channels. */
bool gem_adc_results_ready();


/* The SAMD series has harware correction, which we do use, however, since there are multiple types of
    inputs in play (unbuffered, buffered, some using potentiometers), these functions can apply further
    refinement after hardware adjustment.
*/

#define GEM_CALCULATE_EXPECTED_ADC_CODE(value, range, resolution) \
    ((uint32_t)((value / range) * (resolution - 1)))

struct GemADCErrors {
    /* Offset correction in code points */
    fix16_t offset;
    /* Gain correction as a multiplication factor, generally between 0.5 and 1.5 */
    fix16_t gain;
};

struct GemADCErrors
gem_adc_calculate_errors(uint32_t low_measured, uint32_t low_expected, uint32_t high_measured, uint32_t high_expected);
fix16_t gem_adc_correct_errors(const fix16_t value, const struct GemADCErrors errors);
uint16_t gem_adc_correct_errors_u_int16(const uint16_t value, const struct GemADCErrors errors);
