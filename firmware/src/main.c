#include "gem_adc.h"
#include "gem_clocks.h"
#include "gem_config.h"
#include "gem_gpio.h"
#include "gem_i2c.h"
#include "gem_mcp4728.h"
#include "gem_pulseout.h"
#include "gem_voice_param_table.h"
#include "gem_voice_params.h"
#include "sam.h"
#include <stdio.h>

static void init_pins() {
    gem_gpio_set_as_input(PIN_BUTTON_PORT, PIN_BUTTON, true);
    // gem_gpio_set_as_output(PIN_STATUS_LED_PORT, PIN_STATUS_LED);
    // gem_gpio_set_as_output(PIN_STATUS_LED_2_PORT, PIN_STATUS_LED_2);
}

static uint32_t adc_results[2];

int main(void) {
    /* Configure clocks. */
    gem_clocks_init();

    // Initialize any configuration data and functionality,
    // such as printf() in debug mode.
    gem_config_init();

    init_pins();

    /* Configure the ADC and channel scanning. */
    gem_adc_init();
    gem_adc_init_input(&gem_adc_inputs[0]);
    gem_adc_init_input(&gem_adc_inputs[1]);
    gem_adc_start_scanning(gem_adc_inputs, 2, adc_results);

    /* Configure the timers/PWM generators. */
    gem_pulseout_init();
    gem_pulseout_set_frequency(0, 0xFF);
    gem_pulseout_set_duty(0, 0);
    gem_pulseout_set_frequency(1, 0xFF);
    gem_pulseout_set_duty(1, 0);

    /* Enable i2c bus for communicating with the DAC. */
    gem_i2c_init();

    /* Local variables */
    struct gem_voice_params castor_params;

    while (1) {
        if (gem_adc_results_ready()) {
            printf("Ch1: %lu, Ch2: %lu\r\n", adc_results[0], adc_results[1]);

            gem_pulseout_set_duty(0, adc_results[0] / 4096.0f);
            gem_pulseout_set_duty(1, adc_results[1] / 4096.0f);

            gem_voice_params_from_adc_code(
                gem_voice_param_table, gem_voice_param_table_len, adc_results[1], &castor_params);

            gem_mcp_4728_write_channels(adc_results[0], castor_params.dac_code, adc_results[0], adc_results[1]);
        }
    }

    return 0;
}