#include "fix16.h"
#include "gem.h"
#include "printf.h"
#include "sam.h"

static struct gem_settings settings;
static uint32_t adc_results[GEM_IN_COUNT];
static struct gem_adc_errors knob_errors;
static struct gem_button hard_sync_button = {.port = GEM_HARD_SYNC_BUTTON_PORT, .pin = GEM_HARD_SYNC_BUTTON_PIN};
static bool hard_sync = false;

static struct lfo_state {
    fix16_t phase;
    uint32_t last_update;
} lfo = {.phase = F16(0)};

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
    gem_adc_start_scanning(gem_adc_inputs, GEM_IN_COUNT, adc_results);

    /* Configure the timers/PWM generators. */
    gem_pulseout_init();

    /* Configure input for the hard sync button. */
    gem_button_init(&hard_sync_button);
}

static void calculate_pitch_cv(struct oscillator_state* osc, uint16_t follower_threshold) {
    /*
        The basic pitch CV determination formula is:
        1.0v + (CV in * CV_RANGE) + ((CV knob * KNOB_RANGE) - KNOB_RANGE / 2)
    */

    uint16_t cv_adc_code = (4095 - adc_results[osc->pitch_cv_channel]);
    fix16_t pitch_cv;

    /*
        This allows the second oscillator to follow the first. If the pitch cv
        in is below a certain threshold, the just use "osc->pitch_cv" (which
        will be set to the first oscillator's pitch cv), otherwise, there's
        enough of an input to calculate the pitch cv from the input.
    */
    if (follower_threshold == 0 || cv_adc_code > follower_threshold) {
        fix16_t cv_value = fix16_div(fix16_from_int(cv_adc_code), F16(4095.0));
        pitch_cv = fix16_add(GEM_CV_BASE_OFFSET, fix16_mul(GEM_CV_INPUT_RANGE, cv_value));
    } else {
        pitch_cv = osc->pitch_cv;
    }

    uint16_t knob_adc_code = 4095 - adc_results[osc->pitch_knob_channel];
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
    uint32_t now = gem_get_ticks();
    uint32_t lfo_delta = now - lfo.last_update;

    if (lfo_delta > 0) {
        lfo.phase += fix16_mul(fix16_div(settings.chorus_frequency, F16(1000.0)), fix16_from_int(lfo_delta));

        if (lfo.phase > F16(1.0))
            lfo.phase = fix16_sub(lfo.phase, F16(1.0));

        lfo.last_update = now;
    }

    uint16_t lfo_amount_code = (4095 - adc_results[GEM_IN_CHORUS_POT]);
    fix16_t lfo_amount = fix16_div(fix16_from_int(lfo_amount_code), F16(4095.0));

    fix16_t chorus_lfo_mod = fix16_mul(settings.chorus_max_intensity, fix16_mul(lfo_amount, gem_triangle(lfo.phase)));

    pollux.pitch_cv = fix16_add(pollux.pitch_cv, chorus_lfo_mod);

    /*
        Limit pitch CVs to fit within the parameter table's max value.
    */
    if (castor.pitch_cv < F16(0.0))
        castor.pitch_cv = F16(0.0);
    if (pollux.pitch_cv < F16(0.0))
        pollux.pitch_cv = F16(0.0);
    if (castor.pitch_cv > F16(7.0))
        castor.pitch_cv = F16(7.0);
    if (pollux.pitch_cv > F16(7.0))
        pollux.pitch_cv = F16(7.0);

    /*
        PWM inputs.
    */

    uint16_t castor_duty = 4095 - adc_results[GEM_IN_DUTY_A_POT];
    castor_duty += 4095 - adc_results[GEM_IN_DUTY_A];
    if (castor_duty > 4095)
        castor_duty = 4095;
    uint16_t pollux_duty = 4095 - adc_results[GEM_IN_DUTY_B_POT];
    pollux_duty += 4095 - adc_results[GEM_IN_DUTY_B];
    if (pollux_duty > 4095)
        pollux_duty = 4095;

    /*
        Check for hard sync.
    */
    gem_button_update(&hard_sync_button);

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

int main(void) {
    init();

    while (1) {
        gem_usb_task();
        gem_midi_task();
        gem_led_animation_step();

        if (gem_adc_results_ready()) {
            loop();
        }
    }

    return 0;
}