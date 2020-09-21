#include "gem_adc.h"
#include "gem_clocks.h"
#include "gem_colorspace.h"
#include "gem_config.h"
#include "gem_dotstar.h"
#include "gem_gpio.h"
#include "gem_i2c.h"
#include "gem_mcp4728.h"
#include "gem_midi.h"
#include "gem_nvm.h"
#include "gem_pulseout.h"
#include "gem_quant.h"
#include "gem_spi.h"
#include "gem_systick.h"
#include "gem_usb.h"
#include "gem_voice_param_table.h"
#include "gem_voice_params.h"
#include "gem_waveforms.h"
#include "sam.h"
#include <stdio.h>

static uint32_t adc_results[10];

void midi_event_callback(enum gem_midi_event event);

int main(void) {
    /* Configure clocks. */
    gem_clocks_init();

    /* Configure systick */
    gem_systick_init();

    /* Initialize NVM */
    gem_nvm_init();

    // Initialize any configuration data and functionality,
    // such as printf() in debug mode.
    gem_config_init();

    /* Load settings */
    struct gem_nvm_settings settings;
    bool valid_settings = gem_config_get_nvm_settings(&settings);

    if (!valid_settings) {
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

    /* Enable spi bus for communicating with Dotstars. */
    gem_spi_init();
    gem_dotstar_init(settings.led_brightness);

    /* Configure the ADC and channel scanning. */
    gem_adc_init(settings.adc_offset_corr, settings.adc_gain_corr);

    for (size_t i = 0; i < GEM_IN_COUNT; i++) { gem_adc_init_input(&gem_adc_inputs[i]); }
    gem_adc_start_scanning(gem_adc_inputs, GEM_IN_COUNT, adc_results);

    /* Configure the timers/PWM generators. */
    gem_pulseout_init();

    /* Loop variables. */
    uint32_t last_update = 0;
    uint32_t last_castor_period = 0;
    uint32_t last_pollux_period = 0;
    struct gem_voice_params castor_params = {};
    struct gem_voice_params pollux_params = {};
    float chorus_lfo_phase = 0.0f;
    uint16_t hue = 0;

    while (1) {
        gem_usb_task();
        gem_midi_task();

        hue = gem_get_ticks() * 20;
        for (uint8_t i = 0; i < GEM_DOTSTAR_COUNT; i++) {
            uint32_t color = gem_colorspace_hsv_to_rgb(hue, 255, 255);
            gem_dotstar_set32(i, color);
            hue += 65355 / GEM_DOTSTAR_COUNT;
        }
        gem_dotstar_update();

        if (gem_adc_results_ready()) {
            uint32_t now = gem_get_ticks();
            uint32_t delta = now - last_update;
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

            /* Castor's pitch determination is

                1.0v + quant(CV in) + qaunt(CV knob * 6.0v)

                This means that Castor gets a full range out of
                its pitch input and pitch knob.
            */
            float castor_pitch_cv =
                GEM_CV_BASE_OFFSET +
                gem_quant_pitch_cv((GEM_CV_INPUT_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_A]));
            float castor_pitch_knob =
                (GEM_CASTOR_CV_KNOB_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_A_POT]);
            castor_pitch_cv += gem_quant_pitch_cv(castor_pitch_knob);

            /* Pollux is the "follower", so its pitch determination is based on whether or not
                it has input CV.

                If CV in == 0, then it follows Castor:

                    CV = Castor CV + -1.0v + (2.0v * CV knob)

                Else it uses the input CV:

                    CV = 1.0v + quant(CV in) + -1.0v + (2.0v * CV knob)

                This means that if there's no pitch input, then Pollux is the same pitch as
                Castor but fine-tuned up or down using the CV knob. If there is a pitch CV
                applied, the the knob just acts as a normal fine-tune.
            */
            float pollux_pitch_cv;

            // TODO: Maybe adjust this threshold.
            if (adc_results[GEM_IN_CV_B] > 4090) {
                pollux_pitch_cv = castor_pitch_cv;
            } else {
                pollux_pitch_cv =
                    GEM_CV_BASE_OFFSET +
                    gem_quant_pitch_cv((GEM_CV_INPUT_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_B]));
            }

            float pollux_pitch_knob =
                (-GEM_POLLUX_CV_KNOB_RANGE / 2.0f) +
                (GEM_POLLUX_CV_KNOB_RANGE / 4096.0f) * (float)(4095 - adc_results[GEM_IN_CV_B_POT]);
            pollux_pitch_cv += gem_quant_pitch_knob(pollux_pitch_knob);

            /* Calculate the chorus LFO and account for LFO in Pollux's pitch. */
            float chorus_lfo_amount = adc_results[GEM_IN_CHORUS_POT] / 4096.0f;
            chorus_lfo_phase += GEM_CHORUS_LFO_FREQUENCY / 1000.0f * delta;
            if (chorus_lfo_phase > 1.0f)
                chorus_lfo_phase -= 1.0f;
            float chorus_lfo_mod = 0.01f * chorus_lfo_amount * gem_triangle(chorus_lfo_phase);
            pollux_pitch_cv = castor_pitch_cv;  // temporary
            pollux_pitch_cv += chorus_lfo_mod;

            /* Limit pitch CVs to fit within the parameter table's max value. */
            if(castor_pitch_cv > 7.0f) castor_pitch_cv = 7.0f;
            if(pollux_pitch_cv > 7.0f) pollux_pitch_cv = 7.0f;

            /* TODO: maybe adjust these ranges once tested with new pots. */
            uint16_t castor_duty = 4095 - adc_results[GEM_IN_DUTY_A_POT];
            uint16_t pollux_duty = 4095 - adc_results[GEM_IN_DUTY_B_POT];

            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, castor_pitch_cv, &castor_params);
            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, pollux_pitch_cv, &pollux_params);

            /* Disable interrupts while changing timers, as any interrupt here could totally
                bork the calculations. */
            __disable_irq();
            if (last_castor_period != castor_params.period_reg) {
                gem_pulseout_set_period(0, castor_params.period_reg);
                last_castor_period = castor_params.period_reg;
            }
            if (last_pollux_period != pollux_params.period_reg) {
                gem_pulseout_set_period(1, pollux_params.period_reg);
            }
            __enable_irq();

            // TODO: Enable this once a button is wired up.
            // if(!gem_gpio_get(GEM_IN_SYNC_PORT, GEM_IN_SYNC_PIN)) {
            //     gem_pulseout_hard_sync(true);
            // } else {
            //     gem_pulseout_hard_sync(false);
            // }

            gem_mcp_4728_write_channels(
                (struct gem_mcp4728_channel){.value = castor_params.castor_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = castor_duty},
                (struct gem_mcp4728_channel){.value = pollux_params.pollux_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = pollux_duty});

            last_update = gem_get_ticks();
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