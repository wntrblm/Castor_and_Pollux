#include "fix16.h"
#include "gem.h"
#include "printf.h"
#include "sam.h"
#include <stdlib.h>
#include <string.h>

#define FLIP_ADC(value) (4095 - value)
#define ADC_TO_F16(value) (fix16_div(fix16_from_int(value), F16(4095.0)))
#define UINT12_CLAMP(value) value = value > 4095 ? 4095 : value

static struct gem_settings settings;
static uint32_t adc_results_live[GEM_IN_COUNT];
static uint32_t adc_results_snapshot[GEM_IN_COUNT];
static uint32_t* adc_results = adc_results_live;
static struct gem_adc_errors knob_errors;
static struct gem_button hard_sync_button = {.port = GEM_HARD_SYNC_BUTTON_PORT, .pin = GEM_HARD_SYNC_BUTTON_PIN};
static bool hard_sync = false;
static struct gem_periodic_waveform lfo;

struct oscillator_state {
    enum gem_adc_channels pitch_cv_channel;
    enum gem_adc_channels pitch_knob_channel;
    struct gem_voice_params params;
    struct gem_smoothie_state smooth;
    fix16_t knob_min;
    fix16_t knob_range;
    fix16_t pitch_cv;
};

static struct oscillator_state castor = {
    .params = {},
    .smooth =
        {
            ._lowpass1 = F16(0),
            ._lowpass2 = F16(0),
        },
};

static struct oscillator_state pollux = {
    .params = {},
    .smooth =
        {
            ._lowpass1 = F16(0),
            ._lowpass2 = F16(0),
        },
};

static void init() {
    /* Configure clocks. */
    gem_clocks_init();

    /* Configure systick */
    gem_systick_init();

    /* Initialize NVM */
    gem_nvm_init();

    /* Initialize Random Number Generators */
    gem_random_init(gem_serial_number_low());

    /* Load settings */
    gem_settings_load(&settings);
    gem_settings_print(&settings);

    /* Load the LUT table for DAC codes. */
    gem_load_dac_codes_table();

    /* Initialize USB. */
    gem_usb_init();

    /* Initialize MIDI interface. */
    gem_register_sysex_commands();

    /* Enable i2c bus for communicating with the DAC. */
    gem_i2c_init();

    /* Enable spi bus, Dotstars, and LED animations. */
    gem_spi_init();
    gem_dotstar_init(settings.led_brightness);
    gem_led_animation_init();

    /* Configure the ADC and channel scanning. */
    gem_adc_init(settings.adc_offset_corr, settings.adc_gain_corr);

    for (size_t i = 0; i < GEM_IN_COUNT; i++) { gem_adc_init_input(&gem_adc_inputs[i]); }
    gem_adc_start_scanning(gem_adc_inputs, GEM_IN_COUNT, adc_results_live);

    /* Configure the timers/PWM generators. */
    gem_pulseout_init();

    /* Configure input for the hard sync button. */
    gem_button_init(&hard_sync_button);

    /* Setup LFO. */
    gem_periodic_waveform_init(&lfo, gem_triangle, settings.chorus_frequency);

    /* Setup oscillators. */
    knob_errors = (struct gem_adc_errors){.offset = settings.knob_offset_corr, .gain = settings.knob_gain_corr};

    castor.pitch_cv_channel = GEM_IN_CV_A;
    castor.pitch_knob_channel = GEM_IN_CV_A_POT;
    castor.knob_min = settings.castor_knob_min;
    castor.knob_range = fix16_sub(settings.castor_knob_max, settings.castor_knob_min);
    castor.smooth.initial_gain = settings.smooth_initial_gain;
    castor.smooth.sensitivity = settings.smooth_sensitivity;

    pollux.pitch_cv_channel = GEM_IN_CV_B;
    pollux.pitch_knob_channel = GEM_IN_CV_B_POT;
    pollux.knob_min = settings.pollux_knob_min;
    pollux.knob_range = fix16_sub(settings.pollux_knob_max, settings.pollux_knob_min);
    pollux.smooth.initial_gain = settings.smooth_initial_gain;
    pollux.smooth.sensitivity = settings.smooth_sensitivity;
}

static void calculate_pitch_cv(struct oscillator_state* osc, uint16_t follower_threshold) {
    /*
        The basic pitch CV determination formula is:
        1.0v + (CV in * CV_RANGE) + ((CV knob * KNOB_RANGE) - KNOB_RANGE / 2)
    */

    uint16_t cv_adc_code = FLIP_ADC(adc_results[osc->pitch_cv_channel]);
    fix16_t pitch_cv;

    /*
        This allows the second oscillator to follow the first. If the pitch cv
        in is below a certain threshold, the just use "osc->pitch_cv" (which
        will be set to the first oscillator's pitch cv), otherwise, there's
        enough of an input to calculate the pitch cv from the input.
    */
    if (follower_threshold == 0 || cv_adc_code > follower_threshold) {
        pitch_cv = fix16_add(GEM_CV_BASE_OFFSET, fix16_mul(GEM_CV_INPUT_RANGE, ADC_TO_F16(cv_adc_code)));
    } else {
        pitch_cv = osc->pitch_cv;
    }

    uint16_t knob_adc_code = FLIP_ADC(adc_results[osc->pitch_knob_channel]);
    fix16_t knob_value = fix16_div(gem_adc_correct_errors(fix16_from_int(knob_adc_code), knob_errors), F16(4095.0));
    fix16_t pitch_knob = fix16_add(osc->knob_min, fix16_mul(osc->knob_range, knob_value));

    osc->pitch_cv = fix16_add(pitch_cv, pitch_knob);
}

static void loop() {
    calculate_pitch_cv(&castor, 0);
    pollux.pitch_cv = castor.pitch_cv;
    calculate_pitch_cv(&pollux, settings.pollux_follower_threshold);

    /* Apply smoothing to pitch CVs. */
    castor.pitch_cv = gem_smoothie_step(&castor.smooth, castor.pitch_cv);
    pollux.pitch_cv = gem_smoothie_step(&pollux.smooth, pollux.pitch_cv);

    /*
        Calculate the chorus LFO and account for LFO in Pollux's pitch.
    */
    gem_periodic_waveform_step(&lfo);

    fix16_t lfo_intensity = ADC_TO_F16(FLIP_ADC(adc_results[GEM_IN_CHORUS_POT]));
    fix16_t chorus_lfo_mod = fix16_mul(settings.chorus_max_intensity, fix16_mul(lfo_intensity, lfo.value));

    pollux.pitch_cv = fix16_add(pollux.pitch_cv, chorus_lfo_mod);

    /*
        Limit pitch CVs to fit within the parameter table's max value.
    */
    castor.pitch_cv = fix16_clamp(castor.pitch_cv, F16(0), F16(7));
    pollux.pitch_cv = fix16_clamp(pollux.pitch_cv, F16(0), F16(7));

    /*
        PWM inputs.
    */

    uint16_t castor_duty = FLIP_ADC(adc_results[GEM_IN_DUTY_A_POT]);
    castor_duty += FLIP_ADC(adc_results[GEM_IN_DUTY_A]);
    UINT12_CLAMP(castor_duty);
    uint16_t pollux_duty = FLIP_ADC(adc_results[GEM_IN_DUTY_B_POT]);
    pollux_duty += FLIP_ADC(adc_results[GEM_IN_DUTY_B]);
    UINT12_CLAMP(pollux_duty);

    /*
        Check for hard sync.
    */

    if (gem_button_tapped(&hard_sync_button)) {
        hard_sync = !hard_sync;
        if (hard_sync) {
            gem_pulseout_hard_sync(true);
            gem_led_animation_set_mode(GEM_LED_MODE_HARD_SYNC);
        } else {
            gem_pulseout_hard_sync(false);
            gem_led_animation_set_mode(GEM_LED_MODE_NORMAL);
        }
    }

    /*
        Calculate the final voice parameters given the input CVs.
    */
    gem_voice_params_from_cv(
        gem_voice_voltage_and_period_table,
        gem_voice_dac_codes_table,
        gem_voice_param_table_len,
        castor.pitch_cv,
        &castor.params);
    gem_voice_params_from_cv(
        gem_voice_voltage_and_period_table,
        gem_voice_dac_codes_table,
        gem_voice_param_table_len,
        pollux.pitch_cv,
        &pollux.params);

    /*
        Update timers.
    */

    /* Disable interrupts while changing timers, as any interrupt here could mess them up. */
    __disable_irq();
    gem_pulseout_set_period(0, castor.params.voltage_and_period.period);
    gem_pulseout_set_period(1, pollux.params.voltage_and_period.period);
    __enable_irq();

    /*
        Update DACs.
    */
    gem_mcp_4728_write_channels(
        (struct gem_mcp4728_channel){.value = castor.params.dac_codes.castor, .vref = 1},
        (struct gem_mcp4728_channel){.value = castor_duty},
        (struct gem_mcp4728_channel){.value = pollux.params.dac_codes.pollux, .vref = 1},
        (struct gem_mcp4728_channel){.value = pollux_duty});
}

/* This loop is responsible for dealing with the "tweak"
   interface - when the hard sync button is held down the
   interface knobs allow tweaking various settings. */
static bool tweaking = false;

void tweak_loop() {
    if (gem_button_held(&hard_sync_button)) {
        /* If we just entered tweak mode, copy the adc results to
           the snapshot buffer and point the loop's adc results to
           the snapshot. This prevents the tweak interface for messing
           with the running oscillators. */
        if (!tweaking) {
            tweaking = true;
            memcpy(adc_results_snapshot, adc_results_live, GEM_IN_COUNT * sizeof(uint32_t));
            adc_results = adc_results_snapshot;
            gem_led_animation_set_mode(GEM_LED_MODE_TWEAK);
        }

        /* Chorus intensity knob controls lfo frequency in tweak mode. */
        uint16_t chorus_pot_code = adc_results_live[GEM_IN_CHORUS_POT];
        int32_t chorus_pot_delta = chorus_pot_code - adc_results_snapshot[GEM_IN_CHORUS_POT];
        if (labs(chorus_pot_delta) > 50) {
            fix16_t frequency_value = ADC_TO_F16(FLIP_ADC(chorus_pot_code));
            lfo.frequency = fix16_mul(frequency_value, GEM_TWEAK_MAX_LFO_FREQUENCY);
        }

        /* Update LEDs */
        gem_led_tweak_data.lfo_value = lfo.value;

    } else {
        /* If we just left tweak mode, undo the adc result trickery. */
        if (tweaking) {
            tweaking = false;
            adc_results = adc_results_live;
            gem_led_animation_set_mode(hard_sync ? GEM_LED_MODE_HARD_SYNC : GEM_LED_MODE_NORMAL);
        }
    }
}

int main(void) {
    init();

    while (1) {
        gem_usb_task();
        gem_midi_task();
        gem_led_animation_step();

        if (gem_adc_results_ready()) {
            gem_button_update(&hard_sync_button);
            loop();
            tweak_loop();
        }
    }

    return 0;
}