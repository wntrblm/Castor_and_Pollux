/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_sysex_commands.h"
#include "gem_adc.h"
#include "gem_build_info.h"
#include "gem_config.h"
#include "gem_led_animation.h"
#include "gem_mcp4728.h"
#include "gem_midi_core.h"
#include "gem_pack.h"
#include "gem_pulseout.h"
#include "gem_serial_number.h"
#include "gem_settings.h"
#include "gem_settings_load_save.h"
#include "gem_sysex_dispatcher.h"
#include "gem_usb.h"
#include "gem_voice_param_table.h"
#include "printf.h"
#include "teeth.h"
#include <assert.h>
#include <string.h>

/* Macros & defs */

#define SETTINGS_ENCODED_LEN TEETH_ENCODED_LENGTH(GEMSETTINGS_PACKED_SIZE)
#define CHUNK_SIZE 10
#define TOTAL_CHUNKS (SETTINGS_ENCODED_LEN / CHUNK_SIZE)
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
static_assert(
    SETTINGS_ENCODED_LEN % CHUNK_SIZE == 0, "Total settings encoded length must be a multiple of the chunk size");

/* Static variables. */

static uint8_t _chunk_buf[SETTINGS_ENCODED_LEN];

/* Forward declarations. */

static void _cmd_0x01_hello(const uint8_t* data, size_t len);
static void _cmd_0x02_write_adc_gain(const uint8_t* data, size_t len);
static void _cmd_0x03_write_adc_offset(const uint8_t* data, size_t len);
static void _cmd_0x04_read_adc(const uint8_t* data, size_t len);
static void _cmd_0x05_set_dac(const uint8_t* data, size_t len);
static void _cmd_0x06_set_period(const uint8_t* data, size_t len);
static void _cmd_0x07_erase_settings(const uint8_t* data, size_t len);
static void _cmd_0x08_read_settings(const uint8_t* data, size_t len);
static void _cmd_0x09_write_settings(const uint8_t* data, size_t len);
static void _cmd_0x0A_write_lut_entry(const uint8_t* data, size_t len);
static void _cmd_0x0B_write_lut(const uint8_t* data, size_t len);
static void _cmd_0x0C_erase_lut(const uint8_t* data, size_t len);
static void _cmd_0x0D_disable_adc_corr(const uint8_t* data, size_t len);
static void _cmd_0x0E_enable_adc_corr(const uint8_t* data, size_t len);
static void _cmd_0x0F_get_serial_no(const uint8_t* data, size_t len);

/* Public functions. */

void gem_register_sysex_commands() {
    gem_sysex_register_command(0x01, _cmd_0x01_hello);
    gem_sysex_register_command(0x02, _cmd_0x02_write_adc_gain);
    gem_sysex_register_command(0x03, _cmd_0x03_write_adc_offset);
    gem_sysex_register_command(0x04, _cmd_0x04_read_adc);
    gem_sysex_register_command(0x05, _cmd_0x05_set_dac);
    gem_sysex_register_command(0x06, _cmd_0x06_set_period);
    gem_sysex_register_command(0x07, _cmd_0x07_erase_settings);
    gem_sysex_register_command(0x08, _cmd_0x08_read_settings);
    gem_sysex_register_command(0x09, _cmd_0x09_write_settings);
    gem_sysex_register_command(0x0A, _cmd_0x0A_write_lut_entry);
    gem_sysex_register_command(0x0B, _cmd_0x0B_write_lut);
    gem_sysex_register_command(0x0C, _cmd_0x0C_erase_lut);
    gem_sysex_register_command(0x0D, _cmd_0x0D_disable_adc_corr);
    gem_sysex_register_command(0x0E, _cmd_0x0E_enable_adc_corr);
    gem_sysex_register_command(0x0F, _cmd_0x0F_get_serial_no);
    gem_midi_set_sysex_callback(gem_sysex_dispatcher);
};

static void _cmd_0x01_hello(const uint8_t* data, size_t len) {
    /*
        Response: 0x01 and the build info string, for example:
        "12.24.2020 on 20/01/2021 23:38 UTC with arm-none-eabi-gcc 10.2.1 20201103 (release) by
        stargirl@stargirls-mbp.lan"
    */
    (void)(data);
    (void)(len);

    gem_adc_stop_scanning();
    gem_led_animation_set_mode(GEM_LED_MODE_CALIBRATION);

    const char* build_info = gem_build_info_string();
    size_t data_len = strlen(build_info);
    uint8_t response_buf[200];

    /* Don't copy more of the build info string than we have room for. */
    if (data_len > ARRAY_LEN(response_buf) - 2) {
        data_len = ARRAY_LEN(response_buf) - 2;
    }

    response_buf[0] = GEM_SYSEX_IDENTIFIER;
    response_buf[1] = 0x01;
    memccpy(response_buf + 2, build_info, 0, data_len);

    gem_midi_send_sysex(response_buf, data_len + 2);
}

static void _cmd_0x02_write_adc_gain(const uint8_t* data, size_t len) {
    /* Request (teeth): GAIN(2) */
    (void)(len);

    struct GemSettings settings;
    GemSettings_load(&settings);

    uint8_t request_buf[2];
    teeth_decode(data, TEETH_ENCODED_LENGTH(2), request_buf);

    settings.adc_gain_corr = UNPACK_16(request_buf, 0);

    GemSettings_save(&settings);
}

static void _cmd_0x03_write_adc_offset(const uint8_t* data, size_t len) {
    /* Request (teeth): OFFSET(2) */
    (void)(len);

    struct GemSettings settings;
    GemSettings_load(&settings);

    uint8_t request_buf[2];
    teeth_decode(data, TEETH_ENCODED_LENGTH(2), request_buf);

    settings.adc_offset_corr = UNPACK_16(request_buf, 0);

    GemSettings_save(&settings);
}

static void _cmd_0x04_read_adc(const uint8_t* data, size_t len) {
    /* Request: CHANNEL(1) */
    /* Response (teeth): VALUE(2) */
    (void)(len);

    uint16_t result = gem_adc_read_sync(&gem_adc_inputs[data[0]]);
    uint8_t response[2];

    PACK_16(result, response, 0);

    uint8_t response_buf[2 + TEETH_ENCODED_LENGTH(2)];
    response_buf[0] = GEM_SYSEX_IDENTIFIER;
    response_buf[1] = 0x04;

    teeth_encode(response, 2, response_buf + 2);

    gem_midi_send_sysex(response_buf, ARRAY_LEN(response_buf));
}

static void _cmd_0x05_set_dac(const uint8_t* data, size_t len) {
    /* Request: CHANNEL(1) VALUE(2) VREF(1)*/
    (void)(len);

    uint8_t request_buf[4];
    teeth_decode(data, TEETH_ENCODED_LENGTH(4), request_buf);

    struct GemMCP4278Channel dac_settings = {};
    dac_settings.vref = request_buf[3];
    dac_settings.value = UNPACK_16(request_buf, 1);
    gem_mcp_4728_write_channel(request_buf[0], dac_settings);
}

static void _cmd_0x06_set_period(const uint8_t* data, size_t len) {
    /* Request (teeth): CHANNEL(1) PERIOD(4) */
    (void)(len);

    uint8_t request_buf[5];
    teeth_decode(data, TEETH_ENCODED_LENGTH(5), request_buf);
    gem_pulseout_set_period(request_buf[0], UNPACK_32(request_buf, 1));
}

static void _cmd_0x07_erase_settings(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    GemSettings_erase();
}

static void _cmd_0x08_read_settings(const uint8_t* data, size_t len) {
    /* Settings are sent in chunks to avoid overflowing midi buffers. */
    /* Request: CHUNK_NUM(1) */
    /* Response: SETTINGS_CHUNK(CHUNK_SIZE) */
    (void)(data);
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

    teeth_encode(settings_buf, GEMSETTINGS_PACKED_SIZE, _chunk_buf);

    uint8_t response_buf[CHUNK_SIZE + 2];
    response_buf[0] = GEM_SYSEX_IDENTIFIER;
    response_buf[1] = 0x08;

    memcpy(response_buf + 2, _chunk_buf + (CHUNK_SIZE * chunk_num), CHUNK_SIZE);

    gem_midi_send_sysex(response_buf, ARRAY_LEN(response_buf));
}

static void _cmd_0x09_write_settings(const uint8_t* data, size_t len) {
    /* Settings are sent in chunks to avoid overflowing midi buffers. */
    /* Request: CHUNK_NUM(1) SETTINGS_CHUNK(CHUNK_SIZE) */
    (void)(len);

    const uint8_t chunk_num = data[0];
    if (chunk_num > TOTAL_CHUNKS) {
        printf("Invalid chunk %u.\r\n", chunk_num);
        return;
    }

    if (chunk_num == 0) {
        memset(_chunk_buf, 0xFF, ARRAY_LEN(_chunk_buf));
    }

    memcpy(_chunk_buf + (CHUNK_SIZE * chunk_num), data + 1, CHUNK_SIZE);

    /* All data received, decode and save the settings. */
    if (chunk_num == TOTAL_CHUNKS - 1) {
        struct GemSettings settings;
        uint8_t settings_buf[GEMSETTINGS_PACKED_SIZE];

        teeth_decode(_chunk_buf, ARRAY_LEN(_chunk_buf), settings_buf);

        if (GemSettings_unpack(&settings, settings_buf).status == STRUCTY_RESULT_OKAY) {
            GemSettings_save(&settings);
        } else {
            printf("Failed to save settings, unable to deserialize.\n");
        }
    }

    /* Ack the data. */
    gem_midi_send_sysex((uint8_t[2]){GEM_SYSEX_IDENTIFIER, 0x09}, 2);
}

static void _cmd_0x0A_write_lut_entry(const uint8_t* data, size_t len) {
    /* Request (teeth): ENTRY(1) OSC(1) CODE(4) */
    (void)(len);

    uint8_t request_buf[6];
    teeth_decode(data, 6, request_buf);

    size_t entry = request_buf[0];
    uint8_t osc = request_buf[1];
    uint16_t code = UNPACK_16(request_buf, 2);

    if (entry >= gem_voice_param_table_len) {
        return;
    }

    printf("Set LUT entry %u to %u\r\n", entry, code);

    if (osc == 0) {
        gem_voice_dac_codes_table[entry].castor = code;
    } else {
        gem_voice_dac_codes_table[entry].pollux = code;
    }

    /* Acknowledge the message. */
    gem_midi_send_sysex((uint8_t[2]){GEM_SYSEX_IDENTIFIER, 0x0A}, 2);
}

static void _cmd_0x0B_write_lut(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_save_dac_codes_table();
}

static void _cmd_0x0C_erase_lut(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_save_dac_codes_table();
}

static void _cmd_0x0D_disable_adc_corr(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);
    gem_adc_set_error_correction(2048, 0);
}

static void _cmd_0x0E_enable_adc_corr(const uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    struct GemSettings settings;
    GemSettings_load(&settings);
    gem_adc_set_error_correction(settings.adc_gain_corr, settings.adc_offset_corr);
}

static void _cmd_0x0F_get_serial_no(const uint8_t* data, size_t len) {
    /* Response (teeth): SERIAL_NO(24) */
    (void)(data);
    (void)(len);

    uint8_t response_buf[2 + TEETH_ENCODED_LENGTH(GEM_SERIAL_NUMBER_LEN)];

    response_buf[0] = GEM_SYSEX_IDENTIFIER;
    response_buf[1] = 0x0F;

    uint8_t serial_no[GEM_SERIAL_NUMBER_LEN];
    gem_get_serial_number(serial_no);

    teeth_encode(serial_no, GEM_SERIAL_NUMBER_LEN, response_buf + 2);

    gem_midi_send_sysex(response_buf, ARRAY_LEN(response_buf));
}
