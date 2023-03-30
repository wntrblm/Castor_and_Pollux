/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_sysex.h"
#include "gem_adc.h"
#include "gem_config.h"
#include "gem_led_animation.h"
#include "gem_math.h"
#include "gem_mcp4728.h"
#include "gem_pulseout.h"
#include "gem_ramp_table.h"
#include "gem_settings.h"
#include "gem_settings_load_save.h"
#include "printf.h"
#include "teeth.h"
#include "wntr_assert.h"
#include "wntr_bootloader.h"
#include "wntr_build_info.h"
#include "wntr_midi_core.h"
#include "wntr_midi_sysex_dispatcher.h"
#include "wntr_pack.h"
#include "wntr_serial_number.h"
#include "wntr_ticks.h"
#include "wntr_usb.h"
#include <string.h>

/* Macros & defs */

#ifdef DEBUG
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

#define SETTINGS_ENCODED_LEN TEETH_ENCODED_LENGTH(GEMSETTINGS_PACKED_SIZE)
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

#define DECODE_TEETH_REQUEST(size)                                                                                     \
    WNTR_ASSERT(len == TEETH_ENCODED_LENGTH(size));                                                                    \
    uint8_t request[size];                                                                                             \
    teeth_decode(data, TEETH_ENCODED_LENGTH(size), request);

#define RESPONSE_0(command) wntr_midi_send_sysex((uint8_t[2]){WNTR_MIDI_SYSEX_IDENTIFIER, command}, 2);
#define RESPONSE_1(command, val1) wntr_midi_send_sysex((uint8_t[3]){WNTR_MIDI_SYSEX_IDENTIFIER, command, val1}, 3);

#define PREPARE_RESPONSE(command, size)                                                                                \
    uint8_t _full_response[2 + size];                                                                                  \
    uint8_t* response = _full_response + 2;                                                                            \
    const size_t response_len __attribute__((unused)) = size;                                                          \
    _full_response[0] = WNTR_MIDI_SYSEX_IDENTIFIER;                                                                    \
    _full_response[1] = command;

#define SEND_RESPONSE() wntr_midi_send_sysex(_full_response, ARRAY_LEN(_full_response));

#define SEND_RESPONSE_LEN(len)                                                                                         \
    WNTR_ASSERT(len + 2 <= ARRAY_LEN(_full_response));                                                                 \
    wntr_midi_send_sysex(_full_response, len + 2);

/* Static variables. */

static uint8_t hw_ver_;
const struct GemADCInput* adc_inputs_;
const struct GemI2CConfig* i2c_;
struct GemPulseOutConfig* pulse_;
static bool monitor_enabled_ = false;
static uint32_t last_monitor_update_ = 0;

/* Forward declarations. */

static void cmd_0x01_hello_(const uint8_t* data, size_t len);
static void cmd_0x02_write_adc_gain_(const uint8_t* data, size_t len);
static void cmd_0x03_write_adc_offset_(const uint8_t* data, size_t len);
static void cmd_0x04_read_adc_(const uint8_t* data, size_t len);
static void cmd_0x05_set_dac_(const uint8_t* data, size_t len);
static void cmd_0x07_erase_settings_(const uint8_t* data, size_t len);
static void cmd_0x0A_write_lut_entry_(const uint8_t* data, size_t len);
static void cmd_0x0B_write_lut_(const uint8_t* data, size_t len);
static void cmd_0x0C_erase_lut_(const uint8_t* data, size_t len);
static void cmd_0x0D_disable_adc_corr_(const uint8_t* data, size_t len);
static void cmd_0x0E_enable_adc_corr_(const uint8_t* data, size_t len);
static void cmd_0x0F_get_serial_no_(const uint8_t* data, size_t len);
static void cmd_0x10_monitor_(const uint8_t* data, size_t len);
static void cmd_0x11_soft_reset_(const uint8_t* data, size_t len);
static void cmd_0x12_enter_calibration_mode_(const uint8_t* data, size_t len);
static void cmd_0x13_reset_into_bootloader_(const uint8_t* data, size_t len);
static void cmd_0x18_read_settings_(const uint8_t* data, size_t len);
static void cmd_0x19_write_settings_(const uint8_t* data, size_t len);
static void cmd_0x20_set_frequency_(const uint8_t* data, size_t len);
static void cmd_0x21_set_osc8m_freq_(const uint8_t* data, size_t len);

/* Public functions. */

void gem_sysex_init(
    uint8_t hw_ver,
    const struct GemADCInput* adc_inputs,
    const struct GemI2CConfig* i2c,
    struct GemPulseOutConfig* pulse) {

    hw_ver_ = hw_ver;
    adc_inputs_ = adc_inputs;
    i2c_ = i2c;
    pulse_ = pulse;

    wntr_midi_register_sysex_command(0x01, cmd_0x01_hello_);
    wntr_midi_register_sysex_command(0x02, cmd_0x02_write_adc_gain_);
    wntr_midi_register_sysex_command(0x03, cmd_0x03_write_adc_offset_);
    wntr_midi_register_sysex_command(0x04, cmd_0x04_read_adc_);
    wntr_midi_register_sysex_command(0x05, cmd_0x05_set_dac_);
    wntr_midi_register_sysex_command(0x07, cmd_0x07_erase_settings_);
    wntr_midi_register_sysex_command(0x0A, cmd_0x0A_write_lut_entry_);
    wntr_midi_register_sysex_command(0x0B, cmd_0x0B_write_lut_);
    wntr_midi_register_sysex_command(0x0C, cmd_0x0C_erase_lut_);
    wntr_midi_register_sysex_command(0x0D, cmd_0x0D_disable_adc_corr_);
    wntr_midi_register_sysex_command(0x0E, cmd_0x0E_enable_adc_corr_);
    wntr_midi_register_sysex_command(0x0F, cmd_0x0F_get_serial_no_);
    wntr_midi_register_sysex_command(0x10, cmd_0x10_monitor_);
    wntr_midi_register_sysex_command(0x11, cmd_0x11_soft_reset_);
    wntr_midi_register_sysex_command(0x12, cmd_0x12_enter_calibration_mode_);
    wntr_midi_register_sysex_command(0x13, cmd_0x13_reset_into_bootloader_);
    wntr_midi_register_sysex_command(0x18, cmd_0x18_read_settings_);
    wntr_midi_register_sysex_command(0x19, cmd_0x19_write_settings_);
    wntr_midi_register_sysex_command(0x20, cmd_0x20_set_frequency_);
    wntr_midi_register_sysex_command(0x21, cmd_0x21_set_osc8m_freq_);
};

bool gem_sysex_monitor_enabled() { return monitor_enabled_; }

void gem_sysex_send_monitor_update(struct GemMonitorUpdate* update) {
    if (!monitor_enabled_) {
        return;
    }

    /* Don't send updates more often than once every 1/10th of a second. */
    uint32_t ticks = wntr_ticks();
    if (ticks - last_monitor_update_ < 100) {
        return;
    }
    last_monitor_update_ = ticks;

    uint8_t update_buf[GEMMONITORUPDATE_PACKED_SIZE];
    GemMonitorUpdate_pack(update, update_buf);

    PREPARE_RESPONSE(0x10, TEETH_ENCODED_LENGTH(ARRAY_LEN(update_buf)));
    teeth_encode(update_buf, ARRAY_LEN(update_buf), response);
    SEND_RESPONSE();
}

/* Private functions. */

static void cmd_0x01_hello_(const uint8_t* data, size_t len) {
    /*
        Response: 0x01 and the build info string, for example:
        "12.24.2020 on 20/01/2021 23:38 UTC with arm-none-eabi-gcc 10.2.1 20201103 (release) by
        stargirl@stargirls-mbp.lan"
    */
    (void)(data);
    (void)(len);

    const char* build_info = wntr_build_info_string();
    size_t build_info_len = strlen(build_info);

    PREPARE_RESPONSE(0x01, 128);
    /* Don't copy more of the build info string than we have room for. */
    if (build_info_len > response_len) {
        build_info_len = response_len;
    }
    memccpy(response, build_info, 0, build_info_len);
    SEND_RESPONSE_LEN(build_info_len);

    debug_printf("SysEx 0x01: Hello! Build info length: %u\n", build_info_len);
}

static void cmd_0x02_write_adc_gain_(const uint8_t* data, size_t len) {
    /* Request (teeth): GAIN(2) */
    DECODE_TEETH_REQUEST(2);

    struct GemSettings settings;
    GemSettings_load(&settings);

    settings.adc_gain_corr = WNTR_UNPACK_16(request, 0);

    GemSettings_save(&settings);

    debug_printf("SysEx 0x02: Set ADC gain to %u\n", settings.adc_gain_corr);
}

static void cmd_0x03_write_adc_offset_(const uint8_t* data, size_t len) {
    /* Request (teeth): OFFSET(2) */
    DECODE_TEETH_REQUEST(2);

    struct GemSettings settings;
    GemSettings_load(&settings);

    settings.adc_offset_corr = WNTR_UNPACK_16(request, 0);

    GemSettings_save(&settings);

    debug_printf("SysEx 0x03: Set ADC offset to %u\n", settings.adc_offset_corr);
}

static void cmd_0x04_read_adc_(const uint8_t* data, size_t len) {
    /* Request: CHANNEL(1) */
    /* Response (teeth): VALUE(2) */
    (void)(len);

    uint8_t channel = data[0];
    uint16_t result = gem_adc_read_sync(&adc_inputs_[channel]);

    PREPARE_RESPONSE(0x04, TEETH_ENCODED_LENGTH(2));

    uint8_t unencoded_response[2];
    WNTR_PACK_16(result, unencoded_response, 0);
    teeth_encode(unencoded_response, 2, response);

    SEND_RESPONSE();

    debug_printf("SysEx 0x04: Read ADC channel %u, value %u\n", channel, result);
}

static void cmd_0x05_set_dac_(const uint8_t* data, size_t len) {
    /* Request: VALUE_A(2) VALUE_B(2) VALUE_C(2) VALUE_D(2) */
    DECODE_TEETH_REQUEST(8);

    uint16_t a = WNTR_UNPACK_16(request, 0);
    uint16_t b = WNTR_UNPACK_16(request, 2);
    uint16_t c = WNTR_UNPACK_16(request, 4);
    uint16_t d = WNTR_UNPACK_16(request, 6);

    __attribute__((unused)) enum GemI2CResult res = gem_mcp_4728_write_channels(
        i2c_,
        (struct GemMCP4278Channel){
            .value = a,
        },
        (struct GemMCP4278Channel){
            .value = b,
        },
        (struct GemMCP4278Channel){
            .value = c,
        },
        (struct GemMCP4278Channel){
            .value = d,
        });

    debug_printf("SysEx 0x04: Set DACs to %u, %u, %u, %u. Result: %u\n", a, b, c, d, res);
}

static void cmd_0x07_erase_settings_(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    GemSettings_erase();

    debug_printf("SysEx 0x07: Erased settings\n");
}

static void cmd_0x18_read_settings_(const uint8_t* data, size_t len) {
    /* Response (teeth): serialized settings */
    (void)(data);
    (void)(len);

    struct GemSettings settings;
    uint8_t settings_buf[GEMSETTINGS_PACKED_SIZE];
    GemSettings_load(&settings);
    GemSettings_pack(&settings, settings_buf);

    PREPARE_RESPONSE(0x18, TEETH_ENCODED_LENGTH(GEMSETTINGS_PACKED_SIZE));
    teeth_encode(settings_buf, GEMSETTINGS_PACKED_SIZE, response);
    SEND_RESPONSE();

    debug_printf(
        "SysEx 0x18: Read settings, packed size: %u, encoded size: %u\n",
        GEMSETTINGS_PACKED_SIZE,
        TEETH_ENCODED_LENGTH(GEMSETTINGS_PACKED_SIZE));
}

static void cmd_0x19_write_settings_(const uint8_t* data, size_t len) {
    /* Request (teeth): serialized settings */
    DECODE_TEETH_REQUEST(GEMSETTINGS_PACKED_SIZE);

    struct GemSettings settings;

    if (GemSettings_unpack(&settings, request).status == STRUCTY_RESULT_OKAY) {
        GemSettings_save(&settings);
    } else {
        debug_printf("Failed to save settings, unable to deserialize.\n");
    }

    /* Ack the data. */
    RESPONSE_0(0x19);

    debug_printf("SysEx 0x19: Wrote settings\n");
}

static void cmd_0x0A_write_lut_entry_(const uint8_t* data, size_t len) {
    /* Request (teeth): ENTRY(1) PITCH_CV(4) (unused) CASTOR_CODE(2) POLLUX_CODE(2) */
    DECODE_TEETH_REQUEST(9);

    size_t entry = request[0];
    uint16_t castor_code = WNTR_UNPACK_16(request, 5);
    uint16_t pollux_code = WNTR_UNPACK_16(request, 7);

    if (entry >= gem_ramp_table_len) {
        return;
    }

    gem_ramp_table[entry].castor_ramp_cv = castor_code;
    gem_ramp_table[entry].pollux_ramp_cv = pollux_code;

    /* Acknowledge the message. */
    RESPONSE_0(0x0A);

    debug_printf(
        "SysEX 0x0A: Set LUT entry %u to pitch_cv=UNUSED, castor_ramp_cv=%u, pollux_ramp_cv=%u\n",
        entry,
        castor_code,
        pollux_code);
}

static void cmd_0x0B_write_lut_(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_ramp_table_save();

    debug_printf("SysEx 0x0B: Saved LUT table to NVRAM\n");
}

static void cmd_0x0C_erase_lut_(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_ramp_table_erase();

    debug_printf("SysEx 0x0B: Erased LUT table from NVRAM\n");
}

static void cmd_0x0D_disable_adc_corr_(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_adc_set_error_correction(2048, 0);

    debug_printf("SysEx 0x0D: ADC hardware error correction disabled.\n");
}

static void cmd_0x0E_enable_adc_corr_(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    struct GemSettings settings;
    GemSettings_load(&settings);
    gem_adc_set_error_correction(settings.adc_gain_corr, settings.adc_offset_corr);

    debug_printf("SysEx 0x0e: ADC hardware error correction enabled.\n");
}

static void cmd_0x0F_get_serial_no_(const uint8_t* data, size_t len) {
    /* Response (teeth): SERIAL_NO(24) HARDWARE_VERSION(1) */
    (void)(data);
    (void)(len);

    const size_t raw_response_len = WNTR_SERIAL_NUMBER_LEN + 1;

    PREPARE_RESPONSE(0x0F, TEETH_ENCODED_LENGTH(raw_response_len));

    uint8_t serial_no[raw_response_len];
    wntr_serial_number(serial_no);

    serial_no[raw_response_len - 1] = hw_ver_;

    teeth_encode(serial_no, raw_response_len, response);

    SEND_RESPONSE();

    debug_printf("SysEx 0x0F: Get serial number.\n");
}

static void cmd_0x10_monitor_(const uint8_t* data, size_t len) {
    (void)len;

    if (data[0] > 0) {
        gem_adc_resume_scanning();
        gem_led_animation_set_mode(GEM_MODE_NORMAL);
        monitor_enabled_ = true;
    } else {
        monitor_enabled_ = false;
    }

    debug_printf("SysEx 0x10: Enter monitor mode.\n");
}

static void cmd_0x11_soft_reset_(const uint8_t* data, size_t len) {
    (void)data;
    (void)len;

    debug_printf("SysEx 0x11: Soft reset.\n");

    NVIC_SystemReset();
}

static void cmd_0x12_enter_calibration_mode_(const uint8_t* data, size_t len) {
    (void)data;
    (void)len;

    gem_adc_stop_scanning();
    gem_led_animation_set_mode(GEM_MODE_CALIBRATION);

    debug_printf("SysEx 0x12: Enter calibration mode.\n");
}

static void cmd_0x13_reset_into_bootloader_(const uint8_t* data, size_t len) {
    (void)data;
    (void)len;

    debug_printf("SysEx 0x13: Reset into bootloader.\n");

    wntr_reset_into_bootloader();
}

static void cmd_0x20_set_frequency_(const uint8_t* data, size_t len) {
    /* Request (teeth): CHANNEL(1) FREQUENCY(4) */
    (void)(len);
    DECODE_TEETH_REQUEST(5);

    uint8_t channel = request[0] == 1 ? 0 : 1;
    fix16_t freq_hz = WNTR_UNPACK_32(request, 1);

    uint64_t freq_millihz = gem_frequency_to_millihertz_f16_u64(freq_hz);
    gem_pulseout_set_frequency(pulse_, channel, freq_millihz);

    debug_printf("SysEx 0x20: Set period for osc %u to %u milliHertz\n", channel, freq_millihz);
}

static void cmd_0x21_set_osc8m_freq_(const uint8_t* data, size_t len) {
    /* Request (teeth): FREQUENCY(4) */
    (void)(len);
    DECODE_TEETH_REQUEST(4);

    pulse_->gclk_freq = WNTR_UNPACK_32(request, 0);

    debug_printf("SysEx 0x21: Set pulseout osc8m frequency to %u Hz\n", pulse_->gclk_freq);
}
