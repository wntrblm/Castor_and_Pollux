#include "gem_adc.h"
#include "gem_clocks.h"
#include "gem_config.h"
#include "gem_gpio.h"
#include "gem_i2c.h"
#include "gem_mcp4728.h"
#include "gem_midi.h"
#include "gem_nvm.h"
#include "gem_pulseout.h"
#include "gem_quant.h"
#include "gem_usb.h"
#include "gem_voice_param_table.h"
#include "gem_voice_params.h"
#include "sam.h"
#include <stdio.h>

static uint32_t adc_results[10];

void midi_event_callback(enum gem_midi_event event);

int main(void) {
    /* Configure clocks. */
    gem_clocks_init();

    /* Initialize NVM */
    gem_nvm_init();

    // Initialize any configuration data and functionality,
    // such as printf() in debug mode.
    gem_config_init();

    /* Load settings */
    struct gem_nvm_settings settings;

    if (!gem_config_get_nvm_settings(&settings)) {
        __wrap_printf("Failed to load settings.\r\n");
    } else {
        __wrap_printf("Loaded settings.\r\n");
        printf(
            "Settings: 0x%x, 0x%x, 0x%x\r\n",
            settings.adc_gain_corr,
            settings.adc_offset_corr,
            settings.led_brightness);
    }

    /* Initialize USB. */
    gem_usb_init();

    /* Initialize MIDI interface. */
    gem_midi_set_event_callback(midi_event_callback);

    /* Enable i2c bus for communicating with the DAC. */
    gem_i2c_init();

    /* Configure the ADC and channel scanning. */
    gem_adc_init();
    gem_adc_init_input(&gem_adc_inputs[0]);
    gem_adc_init_input(&gem_adc_inputs[1]);
    gem_adc_init_input(&gem_adc_inputs[2]);
    gem_adc_init_input(&gem_adc_inputs[3]);
    gem_adc_init_input(&gem_adc_inputs[4]);
    gem_adc_init_input(&gem_adc_inputs[5]);
    gem_adc_init_input(&gem_adc_inputs[6]);
    gem_adc_init_input(&gem_adc_inputs[7]);
    gem_adc_init_input(&gem_adc_inputs[8]);
    gem_adc_init_input(&gem_adc_inputs[9]);
    gem_adc_start_scanning(gem_adc_inputs, 10, adc_results);

    /* Configure the timers/PWM generators. */
    gem_pulseout_init();

    /* Test */

    while (1) {
        gem_usb_task();
        gem_midi_task();

        if (gem_adc_results_ready()) {
            // printf(
            //     "CV A: %lu, CV A Pot: %lu, CV B: %lu, CV B Pot: %lu, Duty A: %lu, Duty A Pot: %lu, Duty B: %lu, Duty
            //     B " "Pot: %lu, Phase: %lu, Phase Pot: %lu \r\n", adc_results[0], adc_results[1], adc_results[2],
            //     adc_results[3],
            //     adc_results[4],
            //     adc_results[5],
            //     adc_results[6],
            //     adc_results[7],
            //     adc_results[8],
            //     adc_results[9]);

            struct gem_voice_params castor_params;
            struct gem_voice_params pollux_params;

            /* Castor's pitch determination is
                1.0v + quant(CV in) + qaunt(CV knob * 6.0f)
                This means that Castor gets a full range out of
                its pitch input and pitch knob.
            */
            float castor_pitch_cv =
                GEM_CV_BASE_OFFSET +
                gem_quant_pitch_cv((GEM_CV_INPUT_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_A]));
            float castor_pitch_knob =
                (GEM_CASTOR_CV_KNOB_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_A_POT]);
            castor_pitch_cv += gem_quant_pitch_cv(castor_pitch_knob);

            /* Pollux is the "follower", so its pitch determination is
                1.0f + quant(CV in) + -1.0 + (2.0 * CV knob)
                This means that if there's no pitch input, then Pollux is the same pitch as
                Castor but fine-tuned up or down using the CV knob. If there is a pitch CV
                applied, the the knob just acts as a normal fine-tune.
            */
            float pollux_pitch_cv =
                GEM_CV_BASE_OFFSET +
                gem_quant_pitch_cv((GEM_CV_INPUT_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_B]));
            float pollux_pitch_knob =
                (-GEM_POLLUX_CV_KNOB_RANGE / 2.0f) +
                (GEM_POLLUX_CV_KNOB_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_B_POT]);
            pollux_pitch_cv += pollux_pitch_knob;

            uint16_t castor_duty = 4095 - adc_results[GEM_IN_DUTY_A_POT];
            uint16_t pollux_duty = 4095 - adc_results[GEM_IN_DUTY_B_POT];

            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, castor_pitch_cv, &castor_params);
            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, pollux_pitch_cv, &pollux_params);

            /* Disable interrupts while changing timers, as any interrupt here could totally
                bork the calculations. */
            __disable_irq();
            gem_pulseout_set_period(0, castor_params.period_reg);
            gem_pulseout_set_duty(0, 0.5f);
            gem_pulseout_set_period(1, pollux_params.period_reg);
            gem_pulseout_set_duty(1, 0.5f);
            __enable_irq();

            gem_mcp_4728_write_channels(
                (struct gem_mcp4728_channel){.value = castor_params.castor_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = castor_duty},
                (struct gem_mcp4728_channel){.value = pollux_params.pollux_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = pollux_duty});
        }
    }

    return 0;
}

void midi_event_callback(enum gem_midi_event event) {
    switch (event) {
        case GEM_MIDI_EVENT_CALIBRATION_MODE:
            /* For calibration mode, stop scanning ADC channels. This will also stop
                the main loop above from continuing to change the outputs. */
            gem_adc_stop_scanning();
            break;

        default:
            break;
    }
}