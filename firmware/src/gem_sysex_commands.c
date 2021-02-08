/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_sysex_commands.h"
#include "gem_adc.h"
#include "gem_config.h"
#include "gem_led_animation.h"
#include "gem_lookup_tables.h"
#include "gem_mcp4728.h"
#include "gem_pulseout.h"
#include "gem_serial_number.h"
#include "gem_settings.h"
#include "gem_settings_load_save.h"
#include "gem_sysex_dispatcher.h"
#include "gem_usb.h"
#include "printf.h"
#include "teeth.h"
#include "wntr_assert.h"
#include "wntr_build_info.h"
#include "wntr_midi_core.h"
#include "wntr_pack.h"
#include "wntr_ticks.h"
#include <string.h>

/* Macros & defs */

#define SETTINGS_ENCODED_LEN TEETH_ENCODED_LENGTH(GEMSETTINGS_PACKED_SIZE)
#define CHUNK_SIZE 10
#define TOTAL_CHUNKS (SETTINGS_ENCODED_LEN / CHUNK_SIZE)
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
static_assert(
    SETTINGS_ENCODED_LEN % CHUNK_SIZE == 0, "Total settings encoded length must be a multiple of the chunk size");

#define DECODE_TEETH_REQUEST(size)                                                                                     \
    WNTR_ASSERT(len == TEETH_ENCODED_LENGTH(size));                                                                    \
    uint8_t request[size];                                                                                             \
    teeth_decode(data, TEETH_ENCODED_LENGTH(size), request);

#define RESPONSE_0(command) wntr_midi_send_sysex((uint8_t[2]){GEM_SYSEX_IDENTIFIER, command}, 2);
#define RESPONSE_1(command, val1) wntr_midi_send_sysex((uint8_t[3]){GEM_SYSEX_IDENTIFIER, command, val1}, 3);

#define PREPARE_RESPONSE(command, size)                                                                                \
    uint8_t _full_response[2 + size];                                                                                  \
    uint8_t* response = _full_response + 2;                                                                            \
    const size_t response_len __attribute__((unused)) = size;                                                          \
    _full_response[0] = GEM_SYSEX_IDENTIFIER;                                                                          \
    _full_response[1] = command;

#define SEND_RESPONSE() wntr_midi_send_sysex(_full_response, ARRAY_LEN(_full_response));

#define SEND_RESPONSE_LEN(len)                                                                                         \
    WNTR_ASSERT(len + 2 <= ARRAY_LEN(_full_response));                                                                 \
    wntr_midi_send_sysex(_full_response, len + 2);

/* Static variables. */

static uint8_t chunk_buf_[SETTINGS_ENCODED_LEN];
static bool monitor_enabled_ = false;
static uint32_t last_monitor_update_ = 0;

/* Forward declarations. */

static void cmd_0x01_hello(const uint8_t* data, size_t len);
static void cmd_0x02_write_adc_gain(const uint8_t* data, size_t len);
static void cmd_0x03_write_adc_offset(const uint8_t* data, size_t len);
static void cmd_0x04_read_adc(const uint8_t* data, size_t len);
static void cmd_0x05_set_dac(const uint8_t* data, size_t len);
static void cmd_0x06_set_period(const uint8_t* data, size_t len);
static void cmd_0x07_erase_settings(const uint8_t* data, size_t len);
static void cmd_0x08_read_settings(const uint8_t* data, size_t len);
static void cmd_0x09_write_settings(const uint8_t* data, size_t len);
static void cmd_0x0A_write_lut_entry(const uint8_t* data, size_t len);
static void cmd_0x0B_write_lut(const uint8_t* data, size_t len);
static void cmd_0x0C_erase_lut(const uint8_t* data, size_t len);
static void cmd_0x0D_disable_adc_corr(const uint8_t* data, size_t len);
static void cmd_0x0E_enable_adc_corr(const uint8_t* data, size_t len);
static void cmd_0x0F_get_serial_no(const uint8_t* data, size_t len);
static void cmd_0x10_monitor(const uint8_t* data, size_t len);

/* Public functions. */

void gem_register_sysex_commands() {
    gem_sysex_register_command(0x01, cmd_0x01_hello);
    gem_sysex_register_command(0x02, cmd_0x02_write_adc_gain);
    gem_sysex_register_command(0x03, cmd_0x03_write_adc_offset);
    gem_sysex_register_command(0x04, cmd_0x04_read_adc);
    gem_sysex_register_command(0x05, cmd_0x05_set_dac);
    gem_sysex_register_command(0x06, cmd_0x06_set_period);
    gem_sysex_register_command(0x07, cmd_0x07_erase_settings);
    gem_sysex_register_command(0x08, cmd_0x08_read_settings);
    gem_sysex_register_command(0x09, cmd_0x09_write_settings);
    gem_sysex_register_command(0x0A, cmd_0x0A_write_lut_entry);
    gem_sysex_register_command(0x0B, cmd_0x0B_write_lut);
    gem_sysex_register_command(0x0C, cmd_0x0C_erase_lut);
    gem_sysex_register_command(0x0D, cmd_0x0D_disable_adc_corr);
    gem_sysex_register_command(0x0E, cmd_0x0E_enable_adc_corr);
    gem_sysex_register_command(0x0F, cmd_0x0F_get_serial_no);
    gem_sysex_register_command(0x10, cmd_0x10_monitor);
    wntr_midi_set_sysex_callback(gem_sysex_dispatcher);
};

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

static void cmd_0x01_hello(const uint8_t* data, size_t len) {
    /*
        Response: 0x01 and the build info string, for example:
        "12.24.2020 on 20/01/2021 23:38 UTC with arm-none-eabi-gcc 10.2.1 20201103 (release) by
        stargirl@stargirls-mbp.lan"
    */
    (void)(data);
    (void)(len);

    gem_adc_stop_scanning();
    gem_led_animation_set_mode(GEM_LED_MODE_CALIBRATION);

    const char* build_info = wntr_build_info_string();
    size_t build_info_len = strlen(build_info);

    PREPARE_RESPONSE(0x01, 128);
    /* Don't copy more of the build info string than we have room for. */
    if (build_info_len > response_len) {
        build_info_len = response_len;
    }
    memccpy(response, build_info, 0, build_info_len);
    SEND_RESPONSE_LEN(build_info_len);
}

static void cmd_0x02_write_adc_gain(const uint8_t* data, size_t len) {
    /* Request (teeth): GAIN(2) */
    DECODE_TEETH_REQUEST(2);

    struct GemSettings settings;
    GemSettings_load(&settings);

    settings.adc_gain_corr = WNTR_UNPACK_16(request, 0);

    GemSettings_save(&settings);
}

static void cmd_0x03_write_adc_offset(const uint8_t* data, size_t len) {
    /* Request (teeth): OFFSET(2) */
    DECODE_TEETH_REQUEST(2);

    struct GemSettings settings;
    GemSettings_load(&settings);

    settings.adc_offset_corr = WNTR_UNPACK_16(request, 0);

    GemSettings_save(&settings);
}

static void cmd_0x04_read_adc(const uint8_t* data, size_t len) {
    /* Request: CHANNEL(1) */
    /* Response (teeth): VALUE(2) */
    (void)(len);

    uint16_t result = gem_adc_read_sync(&gem_adc_inputs[data[0]]);

    PREPARE_RESPONSE(0x04, TEETH_ENCODED_LENGTH(2));

    uint8_t unencoded_response[2];
    WNTR_PACK_16(result, unencoded_response, 0);
    teeth_encode(unencoded_response, 2, response);

    SEND_RESPONSE();
}

static void cmd_0x05_set_dac(const uint8_t* data, size_t len) {
    /* Request: CHANNEL(1) VALUE(2) VREF(1)*/
    DECODE_TEETH_REQUEST(4);

    struct GemMCP4278Channel dac_settings = {};
    dac_settings.vref = request[3];
    dac_settings.value = WNTR_UNPACK_16(request, 1);
    gem_mcp_4728_write_channel(request[0], dac_settings);
}

static void cmd_0x06_set_period(const uint8_t* data, size_t len) {
    /* Request (teeth): CHANNEL(1) PERIOD(4) */
    (void)(len);
    DECODE_TEETH_REQUEST(5);

    gem_pulseout_set_period(request[0], WNTR_UNPACK_32(request, 1));
}

static void cmd_0x07_erase_settings(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    GemSettings_erase();
}

static void cmd_0x08_read_settings(const uint8_t* data, size_t len) {
    /* Settings are sent in chunks to avoid overflowing midi buffers. */
    /* Request: CHUNK_NUM(1) */
    /* Response: SETTINGS_CHUNK(CHUNK_SIZE) */
    (void)(len);

    const uint8_t chunk_num = data[0];
    if (chunk_num > TOTAL_CHUNKS) {
        printf("Invalid chunk %u.\r\n", chunk_num);
        return;
    }

    struct GemSettings settings;
    uint8_t settings_buf[GEMSETTINGS_PACKED_SIZE];
    GemSettings_load(&settings);
    GemSettings_pack(&settings, settings_buf);

    teeth_encode(settings_buf, GEMSETTINGS_PACKED_SIZE, chunk_buf_);

    PREPARE_RESPONSE(0x08, CHUNK_SIZE);
    memcpy(response, chunk_buf_ + (CHUNK_SIZE * chunk_num), CHUNK_SIZE);
    SEND_RESPONSE();
}

static void cmd_0x09_write_settings(const uint8_t* data, size_t len) {
    /* Settings are sent in chunks to avoid overflowing midi buffers. */
    /* Request: CHUNK_NUM(1) SETTINGS_CHUNK(CHUNK_SIZE) */
    (void)(len);

    const uint8_t chunk_num = data[0];
    if (chunk_num > TOTAL_CHUNKS) {
        printf("Invalid chunk %u.\r\n", chunk_num);
        return;
    }

    if (chunk_num == 0) {
        memset(chunk_buf_, 0xFF, ARRAY_LEN(chunk_buf_));
    }

    memcpy(chunk_buf_ + (CHUNK_SIZE * chunk_num), data + 1, CHUNK_SIZE);

    /* All data received, decode and save the settings. */
    if (chunk_num == TOTAL_CHUNKS - 1) {
        struct GemSettings settings;
        uint8_t settings_buf[GEMSETTINGS_PACKED_SIZE];

        teeth_decode(chunk_buf_, ARRAY_LEN(chunk_buf_), settings_buf);

        if (GemSettings_unpack(&settings, settings_buf).status == STRUCTY_RESULT_OKAY) {
            GemSettings_save(&settings);
        } else {
            printf("Failed to save settings, unable to deserialize.\n");
        }
    }

    /* Ack the data. */
    RESPONSE_0(0x09);
}

static void cmd_0x0A_write_lut_entry(const uint8_t* data, size_t len) {
    /* Request (teeth): ENTRY(1) PERIOD(4) CASTOR_CODE(2) POLLUX_CODE(2) */
    DECODE_TEETH_REQUEST(9);

    size_t entry = request[0];
    uint32_t period = WNTR_UNPACK_32(request, 1);
    uint16_t castor_code = WNTR_UNPACK_16(request, 5);
    uint16_t pollux_code = WNTR_UNPACK_16(request, 7);

    if (entry >= gem_ramp_table_len) {
        return;
    }

    printf(
        "Set LUT entry %u to period=%u, castor_ramp_cv=%u, pollux_ramp_cv=%u\r\n",
        entry,
        period,
        castor_code,
        pollux_code);

    gem_ramp_table[entry].period = period;
    gem_ramp_table[entry].castor_ramp_cv = castor_code;
    gem_ramp_table[entry].pollux_ramp_cv = pollux_code;

    /* Acknowledge the message. */
    RESPONSE_0(0x0A);
}

static void cmd_0x0B_write_lut(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_save_ramp_table();
}

static void cmd_0x0C_erase_lut(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_erase_ramp_table();
}

static void cmd_0x0D_disable_adc_corr(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_adc_set_error_correction(2048, 0);
}

static void cmd_0x0E_enable_adc_corr(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    struct GemSettings settings;
    GemSettings_load(&settings);
    gem_adc_set_error_correction(settings.adc_gain_corr, settings.adc_offset_corr);
}

static void cmd_0x0F_get_serial_no(const uint8_t* data, size_t len) {
    /* Response (teeth): SERIAL_NO(24) */
    (void)(data);
    (void)(len);

    PREPARE_RESPONSE(0x0F, TEETH_ENCODED_LENGTH(GEM_SERIAL_NUMBER_LEN));

    uint8_t serial_no[GEM_SERIAL_NUMBER_LEN];
    gem_get_serial_number(serial_no);

    teeth_encode(serial_no, GEM_SERIAL_NUMBER_LEN, response);

    SEND_RESPONSE();
}

static void cmd_0x10_monitor(const uint8_t* data, size_t len) {
    (void)len;

    if (data[0] > 0) {
        gem_adc_resume_scanning();
        gem_led_animation_set_mode(GEM_LED_MODE_NORMAL);
        monitor_enabled_ = true;
    } else {
        monitor_enabled_ = false;
    }
}
