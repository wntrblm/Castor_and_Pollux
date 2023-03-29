/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "fix16.h"
#include "gem.h"
#include "printf.h"
#include "sam.h"
#include <stdlib.h>
#include <string.h>

/* Types */

struct KnobsState {
    uint16_t pitch_a;
    bool pitch_a_latch;
    uint16_t pitch_b;
    bool pitch_b_latch;
    uint16_t duty_a;
    bool duty_a_latch;
    uint16_t duty_b;
    bool duty_b_latch;
    uint16_t lfo;
    bool lfo_latch;
};

/* Forward declarations */

static RAMFUNC void init_();
static RAMFUNC void midi_task_();
static RAMFUNC void digital_input_task_();
static RAMFUNC void analog_input_task_();
static RAMFUNC void lfo_task_();
static RAMFUNC void oscillator_task_();
static RAMFUNC void monitor_task_();
static RAMFUNC void pulse_ovf_callback_(uint8_t inst);
static RAMFUNC void update_dac_();
static wntr_periodic_waveform_function lfo_waveshape_setting_to_func_(uint8_t n);

/* Configuration */

static uint32_t board_revision_;
static const struct GemADCConfig* adc_cfg_;
static const struct GemADCInput* adc_inputs_;
static const struct GemOscillatorInputConfig* osc_input_cfg_;
static const struct GemI2CConfig* i2c_cfg_;
static const struct GemSPIConfig* spi_cfg_;
static const struct GemDotstarCfg* dotstar_cfg_;
static const struct GemLEDCfg* led_cfg_;
static struct GemPulseOutConfig pulse_cfg_;

/* Inputs */

static uint32_t adc_results_[GEM_IN_COUNT];
static struct WntrButton button_;
static struct KnobsState knobs_ = {
    .pitch_a = 0,
    .pitch_b = 0,
    .duty_a = 2048,
    .duty_b = 2048,
    .lfo = 0,
    .pitch_a_latch = true,
    .pitch_b_latch = true,
    .duty_a_latch = true,
    .duty_b_latch = true,
    .lfo_latch = true,
};
static struct KnobsState tweak_knobs_ = {
    .pitch_a = UINT16_MAX,
    .pitch_b = UINT16_MAX,
    .duty_a = UINT16_MAX,
    .duty_b = UINT16_MAX,
    .lfo = UINT16_MAX,
};

/* State */

static struct GemSettings settings_;
static struct {
    wntr_periodic_waveform_function functions[2];
    fix16_t frequencies[2];
    fix16_t factors[2];
    fix16_t phases[2];
} lfo_settings_;
static struct WntrMixedPeriodicWaveform lfo_;
static struct GemOscillator castor_;
static struct GemOscillator pollux_;
static struct GemOscillatorInputs castor_inputs_;
static struct GemOscillatorInputs pollux_inputs_;
static enum GemMode mode_ = GEM_MODE_NORMAL;
static bool tweaking_ = false;

/* Timekeeping */

static uint32_t animation_time_ = 0;
static uint32_t sample_time_ = 0;
static uint32_t idle_cycles_ = 0;

/*
    Main, where all things happen.

    Gemini doesn't use an RTOS, instead, it just runs a few tasks that
    are expected to be behave and yield time to other tasks.
*/
int main(void) {
    init_();

    uint32_t last_sample_time = wntr_ticks();

    while (1) {
        wntr_usb_task();
        midi_task_();
        digital_input_task_();
        lfo_task_();

        // The LED animation task internally ensures that it only runs once
        // every few milliseconds. See GEM_ANIMATION_INTERVAL.
        uint32_t animation_start_time = wntr_ticks();
        if (gem_led_animation_step(dotstar_cfg_)) {
            animation_time_ = wntr_ticks() - animation_start_time;
        }

        // The analog input, oscillator, and monitor tasks only need to be
        // called when there's a new set of ADC readings ready. The ADC is
        // constantly scanning in the background, so that gives the USB, MIDI,
        // and LED animation tasks time to run between oscillator updates.
        if (gem_adc_results_ready()) {
            sample_time_ = (uint16_t)(wntr_ticks() - last_sample_time);
            last_sample_time = wntr_ticks();
            analog_input_task_();
            oscillator_task_();
            monitor_task_();
            idle_cycles_ = 0;
        } else {
            idle_cycles_++;
        }
    }

    return 0;
}

/*
    Initializes the core processor, clocks, peripherals, drivers, settings,
    and global state.
*/
static void init_() {
    //
    // Hardware revision check
    //

    // Since the peripheral configuration depends on the hardware revision,
    // the firmware must detect which hardware revision. This is done by
    // connecting otherwise unused GPIO pins to ground. The firmware checks
    // the value of those pins to determine the hardware revision.
    WntrGPIOPin_set_as_input(GEM_REV5_PIN, true);

    // rev5 pin is floating for revisions < 5, so it gets pulled up.
    if (WntrGPIOPin_get(GEM_REV5_PIN) == true) {
        board_revision_ = 4;
        adc_cfg_ = &GEM_ADC_CFG;
        adc_inputs_ = GEM_REV1_ADC_INPUTS;
        osc_input_cfg_ = &GEM_REV1_OSC_INPUT_CFG;
        pulse_cfg_ = GEM_REV1_PULSE_OUT_CFG;
        i2c_cfg_ = &GEM_REV1_I2C_CFG;
        spi_cfg_ = &GEM_REV1_SPI_CFG;
        dotstar_cfg_ = &GEM_REV1_DOTSTAR_CFG;
        led_cfg_ = &GEM_REV1_LED_CFG;
    }

    // rev5 pin is tied to ground in revisions > 5.
    else {
        board_revision_ = 5;
        adc_cfg_ = &GEM_ADC_CFG;
        adc_inputs_ = GEM_REV5_ADC_INPUTS;
        osc_input_cfg_ = &GEM_REV5_OSC_INPUT_CFG;
        pulse_cfg_ = GEM_REV5_PULSE_OUT_CFG;
        i2c_cfg_ = &GEM_REV5_I2C_CFG;
        spi_cfg_ = &GEM_REV5_SPI_CFG;
        dotstar_cfg_ = &GEM_REV5_DOTSTAR_CFG;
        led_cfg_ = &GEM_REV5_LED_CFG;
    }

    // Tell the world who we are and how we got here. :)
    printf("Hello, I am Gemini.\n - hardware: rev%u\n - firmware: %s\n", board_revision_, wntr_build_info_string());

    // Gemini uses a pseudo-random number generator for the LED animation.
    // To keep things simple, it just uses its serial number as the seed.
    // If it needs to be more fancy in the future it could be changed to read
    // a floating ADC input and use that as the seed.
    uint8_t serial_number[WNTR_SERIAL_NUMBER_LEN];
    wntr_serial_number(serial_number);
    wntr_random_init(*((uint32_t*)(serial_number)));

    //
    // Load persistent configuration stored in non-volatile RAM
    //

    // Gemini stores the user configurable settings in NVM so they have to be
    // explicitly loaded.
    GemSettings_load(&settings_);
    GemSettings_print(&settings_);

    // Gemini also stores a ramp table in NVM. This table is used to
    // compensate for amplitude loss in the ramp waveform as frequency
    // increases.
    gem_ramp_table_load();

    //
    // Peripheral setup
    //

    // Gemini uses USB MIDI for editing settings and factory configuration.
    wntr_usb_init();

    // Gemini uses i2c to communicate with the external DAC.
    gem_i2c_init(i2c_cfg_);
    gem_mcp_4728_init(i2c_cfg_);

    // Gemini uses SPI to communicate with the Dotstar LEDs.
    gem_spi_init(spi_cfg_);

    //
    // Driver configuration
    //

    /* Register SysEx commands used for factory setup. */
    gem_sysex_init(adc_inputs_, i2c_cfg_, &pulse_cfg_);

    /* Enable the Dotstar driver and LED animation. */
    gem_dotstar_init(settings_.led_brightness);
    gem_led_animation_init(*led_cfg_);
    gem_led_animation_set_mode(mode_);

    // Set up the SAMD21's ADC.
    //
    // First, use the digital offset and gain error correction which is
    // measured during assembly and stored in the user settings. This gives
    // Gemini significantly more accurate readings.
    gem_adc_init(adc_cfg_, settings_.adc_offset_corr, settings_.adc_gain_corr);

    // Second, measure all of the input channels in the background using
    // "channel scanning". This frees up the main loop to do other things
    // while waiting for new measurements for all the channels.
    for (size_t i = 0; i < GEM_IN_COUNT; i++) { gem_adc_init_input(&(adc_inputs_[i])); }
    gem_adc_start_scanning(adc_inputs_, GEM_IN_COUNT, adc_results_);

    // The WntrButton helper is used for the panel button so Gemini can check
    // if it's tapped or held.
    button_.pin = button_pin_.pin;
    button_.port = button_pin_.port;
    WntrButton_init(&button_);

    //
    // Oscillator configuration and initialization.
    //

    // Gemini has an internal low-frequency oscillator that can be used to
    // modulate the pitch and pulse width of the primary oscillators.
    lfo_settings_.functions[0] = lfo_waveshape_setting_to_func_(settings_.lfo_1_waveshape);
    lfo_settings_.functions[1] = lfo_waveshape_setting_to_func_(settings_.lfo_2_waveshape);
    lfo_settings_.frequencies[0] = settings_.lfo_1_frequency;
    lfo_settings_.frequencies[1] = fix16_mul(settings_.lfo_1_frequency, settings_.lfo_2_frequency_ratio);
    lfo_settings_.factors[0] = settings_.lfo_1_factor;
    lfo_settings_.factors[1] = settings_.lfo_2_factor;
    lfo_settings_.phases[0] = F16(0);
    lfo_settings_.phases[1] = F16(0);
    WntrMixedPeriodicWaveform_init(
        &lfo_,
        2,
        lfo_settings_.functions,
        lfo_settings_.frequencies,
        lfo_settings_.factors,
        lfo_settings_.phases,
        wntr_ticks());

    // Gemini has two oscillators - Castor & Pollux. For the most part they're
    // completely independent: they each have their own pitch and pulse width
    // inputs, their own pitch knob range configuration, and their own
    // dedicated outputs.
    //
    // They share a small amount of common configuration: the ADC error
    // calibration data and pitch knob non-linearity setting.
    gem_oscillator_init(
        (struct WntrErrorCorrection){.offset = settings_.cv_offset_error, .gain = settings_.cv_gain_error},
        settings_.pitch_knob_nonlinearity);

    castor_ = (struct GemOscillator){
        .number = 0,
        .pitch_offset = settings_.base_cv_offset,
        .pitch_cv_min = osc_input_cfg_->pitch_cv_min,
        .pitch_cv_max = osc_input_cfg_->pitch_cv_max,
        .lfo_pitch_factor = settings_.chorus_max_intensity,
        .pitch_knob_min = settings_.castor_knob_min,
        .pitch_knob_max = settings_.castor_knob_max,
        .pulse_width_bitmask = settings_.pulse_width_bitmask,
        .can_follow = false,
        .zero_detection_enabled = settings_.zero_detection_enabled,
        .zero_detection_threshold = settings_.zero_detection_threshold,
        .quantization_enabled = settings_.quantization_enabled,
    };
    GemOscillator_init(&castor_);

    pollux_ = (struct GemOscillator){
        .number = 1,
        .pitch_offset = settings_.base_cv_offset,
        .pitch_cv_min = osc_input_cfg_->pitch_cv_min,
        .pitch_cv_max = osc_input_cfg_->pitch_cv_max,
        .lfo_pitch_factor = settings_.chorus_max_intensity,
        .pitch_knob_min = settings_.pollux_knob_min,
        .pitch_knob_max = settings_.pollux_knob_max,
        .pulse_width_bitmask = settings_.pulse_width_bitmask,
        // If Pollux doesn't have any pitch CV input it'll follow Castor's
        // pitch. Rev 1-4 detects lack of pitch CV input by checking if Pollux's
        // pitch CV is near zero. Rev 5 has a switched jack, but still does
        // the near zero check to follow Castor when both pitch inputs are
        // unpatched.
        .zero_detection_enabled = settings_.zero_detection_enabled,
        .zero_detection_threshold = settings_.zero_detection_threshold,
    };
    GemOscillator_init(&pollux_);

    // Configure the SAMD21's TCC peripheral to output the square waves needed
    // by the oscillators' ramp core.
    pulse_cfg_.gclk_freq = settings_.osc8m_freq;
    gem_pulseout_init(&pulse_cfg_, pulse_ovf_callback_);
}

/*
    This task deals with digital inputs, which in Gemini's case, is just the
    one button on the panel. This button has two purposes: if tapped, it
    cycles through Gemini's modes (normal, lfo -> fm, lfo -> pwm, and hard
    sync), if held it activates "tweak" mode which maps the knobs to a
    different set of mode-specific parameters.
*/
static RAMFUNC void digital_input_task_() {
    WntrButton_update(&button_);

    // If the button was just tapped the change to the next mode.
    if (WntrButton_tapped(&button_)) {
        mode_ = (mode_ + 1) % GEM_MODE_COUNT;
        gem_led_animation_set_mode(mode_);
    }

    // If we just entered tweak mode, clear all of the tweak knobs latches
    // so that paramters don't change until the user moves a knob.
    if (WntrButton_hold_started(&button_)) {
        tweaking_ = true;
        tweak_knobs_.pitch_a_latch = false;
        tweak_knobs_.pitch_b_latch = false;
        tweak_knobs_.duty_a_latch = false;
        tweak_knobs_.duty_b_latch = false;
        tweak_knobs_.lfo_latch = false;
        gem_led_inputs.tweaking = tweaking_;
    }

    // If we just left tweak mode, clear all the regular knob latches so that
    // those parameters don't immediately change.
    if (WntrButton_hold_ended(&button_)) {
        tweaking_ = false;
        knobs_.pitch_a_latch = false;
        knobs_.pitch_b_latch = false;
        knobs_.duty_a_latch = false;
        knobs_.duty_b_latch = false;
        knobs_.lfo_latch = false;
        gem_led_inputs.tweaking = tweaking_;
    }
}

/*
    This task handles processing the ADC and such into the input states.
*/
static RAMFUNC void analog_input_task_() {
    // Update the knobs structs with the state of the ADC inputs. Since Gemini
    // has a "tweak" mode (where you hold down the button) this has to handle
    // swapping between which knobset is active. When switching between modes,
    // the knob state doesn't update until one of the knobs is moved enough
    // to register a change.
    struct KnobsState* active_knobs = &knobs_;
    struct KnobsState* inactive_knobs = &tweak_knobs_;

    if (tweaking_) {
        active_knobs = &tweak_knobs_;
        inactive_knobs = &knobs_;
    }

#define KNOB_UPDATE(name, channel)                                                                                     \
    if (abs((int32_t)(inactive_knobs->name) - (int32_t)(adc_results_[channel])) > 20)                                  \
        active_knobs->name##_latch = true;                                                                             \
    if (active_knobs->name##_latch)                                                                                    \
        active_knobs->name = adc_results_[channel];

    KNOB_UPDATE(pitch_a, GEM_IN_CV_A_POT);
    KNOB_UPDATE(pitch_b, GEM_IN_CV_B_POT);
    KNOB_UPDATE(duty_a, GEM_IN_DUTY_A_POT);
    KNOB_UPDATE(duty_b, GEM_IN_DUTY_B_POT);
    KNOB_UPDATE(lfo, GEM_IN_CHORUS_POT);

    gem_led_inputs.pitch_tweak_a = tweak_knobs_.pitch_a;
    gem_led_inputs.pitch_tweak_b = tweak_knobs_.pitch_b;
}

/*
    This task handles updating the internal LFO
*/

static RAMFUNC void lfo_task_() {
    // Update the internal LFO parameters based on the mode
    // In normal mode and hard sync mode, the tweak mode LFO knob controls the LFO frequency.
    // In the LFO alt modes, the LFO knob controls the LFO frequency.

    fix16_t lfo_frequency = F16(0);

    if (mode_ == GEM_MODE_NORMAL || mode_ == GEM_MODE_HARD_SYNC) {
        if (tweak_knobs_.lfo != UINT16_MAX) {
            lfo_frequency = fix16_mul(UINT12_NORMALIZE(tweak_knobs_.lfo), GEM_TWEAK_MAX_LFO_FREQ);
        } else {
            lfo_frequency = settings_.lfo_1_frequency;
        }
    } else {
        lfo_frequency = fix16_mul(UINT12_NORMALIZE(knobs_.lfo), GEM_TWEAK_MAX_LFO_FREQ);
    }
    lfo_settings_.frequencies[0] = lfo_frequency;

    // Advance the LFO one step
    WntrMixedPeriodicWaveform_step(&lfo_, wntr_ticks());

    // Tell the LED animation about the LFO values, since it uses it to control
    // the animations.
    gem_led_inputs.lfo_amplitude = lfo_.amplitude;
    gem_led_inputs.lfo_gain = UINT12_NORMALIZE(knobs_.lfo);
    gem_led_inputs.lfo_mod_a = knobs_.duty_a;
    gem_led_inputs.lfo_mod_b = knobs_.duty_b;
}

/*
    This task handles taking in the input state (from the ADC and such) and
    updating the oscillators, recalculating their outputs, and applying the
    outputs to the pulse generators and DACs.
*/

static RAMFUNC void oscillator_task_() {

    // Update both oscillator's internal state based on the ADC inputs.
    castor_inputs_.mode = mode_;
    castor_inputs_.pitch_cv_code = adc_results_[GEM_IN_CV_A];
    castor_inputs_.pitch_knob_code = knobs_.pitch_a;
    castor_inputs_.tweak_pitch_knob_code = tweak_knobs_.pitch_a;
    castor_inputs_.pulse_cv_code = adc_results_[GEM_IN_DUTY_A];
    castor_inputs_.pulse_knob_code = knobs_.duty_a;
    castor_inputs_.tweak_pulse_knob_code = tweak_knobs_.duty_a;
    castor_inputs_.lfo_knob_code = knobs_.lfo;
    castor_inputs_.tweak_lfo_knob_code = tweak_knobs_.lfo;
    castor_inputs_.reference_pitch = F16(0);
    castor_inputs_.lfo_amplitude = lfo_.amplitude;

    GemOscillator_update(&castor_, castor_inputs_);

    pollux_inputs_.mode = mode_;
    pollux_inputs_.pitch_cv_code = adc_results_[GEM_IN_CV_B];
    pollux_inputs_.pitch_knob_code = knobs_.pitch_b;
    pollux_inputs_.tweak_pitch_knob_code = tweak_knobs_.pitch_b;
    pollux_inputs_.pulse_cv_code = adc_results_[GEM_IN_DUTY_B];
    pollux_inputs_.pulse_knob_code = knobs_.duty_b;
    pollux_inputs_.tweak_pulse_knob_code = tweak_knobs_.duty_b;
    pollux_inputs_.lfo_knob_code = knobs_.lfo;
    pollux_inputs_.tweak_lfo_knob_code = tweak_knobs_.lfo;
    pollux_inputs_.reference_pitch = castor_.pitch;
    pollux_inputs_.lfo_amplitude = lfo_.amplitude;

    GemOscillator_update(&pollux_, pollux_inputs_);

    // Oscillator post-update applies final values to the oscillator state.
    GemOscillator_post_update(&pulse_cfg_, &castor_);
    GemOscillator_post_update(&pulse_cfg_, &pollux_);

    // Update the timers with their new values calculated from their
    // oscillator's pitch.
    //
    // It's important that these get updated at essentially the same time so
    // that they have a stable phase relationship. Therefore, interrupts are
    // disabled while Gemini modifies the timer configuration.
    __disable_irq();
    gem_pulseout_set_period(&pulse_cfg_, 0, castor_.pulseout_period);
    gem_pulseout_set_period(&pulse_cfg_, 1, pollux_.pulseout_period);
    __enable_irq();

    update_dac_();
}

static RAMFUNC void monitor_task_() {
    static uint16_t last_loop_time_ = 0;

    if (!gem_sysex_monitor_enabled()) {
        return;
    }

    // To help with testing and debugging, Gemini can send its state over
    // MIDI SysEx to the monitoring script in `/factory/monitor.py`.
    uint16_t loop_time = (uint16_t)(wntr_ticks() - last_loop_time_);

    struct GemMonitorUpdate monitor_update = {
        .mode = mode_,

        .tweaking = tweaking_,
        .lfo_knob = knobs_.lfo,
        .tweak_lfo_knob = tweak_knobs_.lfo,

        .castor_pitch_knob = castor_inputs_.pitch_knob_code,
        .castor_pitch_cv = castor_inputs_.pitch_cv_code,
        .castor_pulse_knob = castor_inputs_.pulse_knob_code,
        .castor_pulse_cv = castor_inputs_.pulse_cv_code,
        .castor_tweak_pitch_knob = castor_inputs_.tweak_pitch_knob_code,
        .castor_tweak_pulse_knob = castor_inputs_.tweak_pulse_knob_code,

        .castor_pitch_behavior = castor_.pitch_behavior,
        .castor_pitch = castor_.pitch,
        .castor_pulse_width = castor_.pulse_width,
        .castor_period = castor_.pulseout_period,
        .castor_ramp = castor_.ramp_cv,

        .pollux_pitch_knob = pollux_inputs_.pitch_knob_code,
        .pollux_pitch_cv = pollux_inputs_.pitch_cv_code,
        .pollux_pulse_knob = pollux_inputs_.pulse_knob_code,
        .pollux_pulse_cv = pollux_inputs_.pulse_cv_code,
        .pollux_tweak_pitch_knob = pollux_inputs_.tweak_pitch_knob_code,
        .pollux_tweak_pulse_knob = pollux_inputs_.tweak_pulse_knob_code,

        .pollux_reference_pitch = pollux_inputs_.reference_pitch,
        .pollux_pitch_behavior = pollux_.pitch_behavior,
        .pollux_pitch = pollux_.pitch,
        .pollux_pulse_width = pollux_.pulse_width,
        .pollux_period = pollux_.pulseout_period,
        .pollux_ramp = pollux_.ramp_cv,

        .loop_time = loop_time,
        .animation_time = (uint16_t)(animation_time_),
        .sample_time = (uint16_t)(idle_cycles_)};

    gem_sysex_send_monitor_update(&monitor_update);

    last_loop_time_ = wntr_ticks();
}

/*
    Handles incoming MIDI messages and dispatches them to the SysEx handlers.
*/
static void midi_task_() {
    struct WntrMIDIMessage msg = {};
    if (!wntr_midi_receive(&msg)) {
        return;
    }
    if (msg.code_index == MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE) {
        wntr_midi_dispatch_sysex();
    }
}

/*
    This is called when the TCC peripheral controlling Castor overflows its
    counter. This is used to reset the TCC controlling Pollux to achieve the
    "hard sync" effect.
*/
static void pulse_ovf_callback_(uint8_t inst) {
    (void)inst;
    if (mode_ == GEM_MODE_HARD_SYNC) {
        TCC1->CTRLBSET.reg = TCC_CTRLBSET_CMD_RETRIGGER;
    }
}

/*
    Update the DAC outputs with the new charge and pulse width
    values.
*/
static inline __attribute__((always_inline)) void update_dac_() {
    // Each oscillator requires two DAC outputs.
    //
    // The first one is used to compensate for amplitude loss as frequency
    // increases. Higher voltage allows the ramp core's integrating capacitor
    // to charge more quickly and reach a higher voltage before the timer
    // resets the ramp.
    //
    // The second is used to control the pulse-width of the pulse waveform.
    // The output voltage goes into a comparator that compares against the
    // ramp waveform to generate a pulse.

    if (board_revision_ >= 5) {
        gem_mcp_4728_write_channels(
            i2c_cfg_,
            (struct GemMCP4278Channel){.value = pollux_.ramp_cv},
            (struct GemMCP4278Channel){.value = pollux_.pulse_width},
            (struct GemMCP4278Channel){.value = castor_.ramp_cv},
            (struct GemMCP4278Channel){.value = castor_.pulse_width});
    } else {
        gem_mcp_4728_write_channels(
            i2c_cfg_,
            (struct GemMCP4278Channel){.value = castor_.ramp_cv},
            (struct GemMCP4278Channel){.value = castor_.pulse_width},
            (struct GemMCP4278Channel){.value = pollux_.ramp_cv},
            (struct GemMCP4278Channel){.value = pollux_.pulse_width});
    }
}

static wntr_periodic_waveform_function lfo_waveshape_setting_to_func_(uint8_t n) {
    switch (n) {
        case 0:
            return wntr_triangle;
        case 1:
            return wntr_sine;
        case 2:
            return wntr_sawtooth;
        case 3:
            return wntr_square;
        default:
            return wntr_triangle;
    }
}
