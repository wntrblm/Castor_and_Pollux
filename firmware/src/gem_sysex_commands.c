#include "gem_sysex_commands.h"
#include "gem_adc.h"
#include "gem_config.h"
#include "gem_led_animation.h"
#include "gem_mcp4728.h"
#include "gem_midi_core.h"
#include "gem_pack.h"
#include "gem_pulseout.h"
#include "gem_settings.h"
#include "gem_usb.h"
#include "gem_voice_param_table.h"
#include "printf.h"
#include "teeth.h"
#include <string.h>

/* Macros & defs */

#define MAX_SETTINGS_SIZE 64
#define TOTAL_BYTES TEETH_ENCODED_LENGTH(MAX_SETTINGS_SIZE)
#define CHUNK_SIZE 20
#define TOTAL_CHUNKS (TOTAL_BYTES / CHUNK_SIZE)

/* Static variables. */

static uint8_t _working_buf[128];
static uint8_t _chunk_buf[TOTAL_BYTES];

/* Forward declarations. */

static void _cmd_0x01_hello(uint8_t* data, size_t len);
static void _cmd_0x02_write_adc_gain(uint8_t* data, size_t len);
static void _cmd_0x03_write_adc_offset(uint8_t* data, size_t len);
static void _cmd_0x04_read_adc(uint8_t* data, size_t len);
static void _cmd_0x05_set_dac(uint8_t* data, size_t len);
static void _cmd_0x06_set_period(uint8_t* data, size_t len);
static void _cmd_0x07_erase_settings(uint8_t* data, size_t len);
static void _cmd_0x08_read_settings(uint8_t* data, size_t len);
static void _cmd_0x09_write_settings(uint8_t* data, size_t len);
static void _cmd_0x0A_write_lut_entry(uint8_t* data, size_t len);
static void _cmd_0x0B_write_lut(uint8_t* data, size_t len);
static void _cmd_0x0C_erase_lut(uint8_t* data, size_t len);
static void _cmd_0x0D_disable_adc_corr(uint8_t* data, size_t len);
static void _cmd_0x0E_enable_adc_corr(uint8_t* data, size_t len);

/* Public functions. */

void gem_register_sysex_commands() {
    gem_midi_register_sysex_command(0x01, _cmd_0x01_hello);
    gem_midi_register_sysex_command(0x02, _cmd_0x02_write_adc_gain);
    gem_midi_register_sysex_command(0x03, _cmd_0x03_write_adc_offset);
    gem_midi_register_sysex_command(0x04, _cmd_0x04_read_adc);
    gem_midi_register_sysex_command(0x05, _cmd_0x05_set_dac);
    gem_midi_register_sysex_command(0x06, _cmd_0x06_set_period);
    gem_midi_register_sysex_command(0x07, _cmd_0x07_erase_settings);
    gem_midi_register_sysex_command(0x08, _cmd_0x08_read_settings);
    gem_midi_register_sysex_command(0x09, _cmd_0x09_write_settings);
    gem_midi_register_sysex_command(0x0A, _cmd_0x0A_write_lut_entry);
    gem_midi_register_sysex_command(0x0B, _cmd_0x0B_write_lut);
    gem_midi_register_sysex_command(0x0C, _cmd_0x0C_erase_lut);
    gem_midi_register_sysex_command(0x0D, _cmd_0x0D_disable_adc_corr);
    gem_midi_register_sysex_command(0x0E, _cmd_0x0E_enable_adc_corr);
};

static void _cmd_0x01_hello(uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_adc_stop_scanning();
    gem_led_animation_set_mode(GEM_LED_MODE_CALIBRATION);

    gem_midi_send_sysex((uint8_t[3]){GEM_MIDI_SYSEX_MARKER, 0x01, GEM_FIRMWARE_VERSION}, 3);
}

static void _cmd_0x02_write_adc_gain(uint8_t* data, size_t len) {
    /* Request (teeth): GAIN(2) */
    (void)(len);

    struct gem_settings settings;
    gem_settings_load(&settings);

    teeth_decode(data + 2, TEETH_ENCODED_LENGTH(2), _working_buf);

    settings.adc_gain_corr = UNPACK_16(_working_buf, 0);

    gem_settings_save(&settings);
}

static void _cmd_0x03_write_adc_offset(uint8_t* data, size_t len) {
    /* Request (teeth): OFFSET(2) */
    (void)(len);

    struct gem_settings settings;
    gem_settings_load(&settings);

    teeth_decode(data + 2, TEETH_ENCODED_LENGTH(2), _working_buf);

    settings.adc_offset_corr = UNPACK_16(_working_buf, 0);

    gem_settings_save(&settings);
}

static void _cmd_0x04_read_adc(uint8_t* data, size_t len) {
    /* Request: CHANNEL(1) */
    /* Response (teeth): VALUE(2) */
    (void)(len);

    uint16_t result = gem_adc_read_sync(&gem_adc_inputs[data[2]]);
    uint8_t response[2];

    PACK_16(result, response, 0);

    _working_buf[0] = GEM_MIDI_SYSEX_MARKER;
    _working_buf[1] = 0x04;

    teeth_encode(response, 2, _working_buf + 2);

    gem_midi_send_sysex(_working_buf, TEETH_ENCODED_LENGTH(2));
}

static void _cmd_0x05_set_dac(uint8_t* data, size_t len) {
    /* Request: VREF(1) CHANNEL(1) VALUE(2) */
    (void)(len);

    teeth_decode(data + 2, TEETH_ENCODED_LENGTH(4), _working_buf);

    struct gem_mcp4728_channel dac_settings = {};
    dac_settings.vref = _working_buf[0];
    dac_settings.value = UNPACK_16(_working_buf, 2);
    gem_mcp_4728_write_channel(_working_buf[1], dac_settings);
}

static void _cmd_0x06_set_period(uint8_t* data, size_t len) {
    /* Request (teeth): CHANNEL(1) PERIOD(4) */
    (void)(len);

    teeth_decode(data + 2, TEETH_ENCODED_LENGTH(5), _working_buf);
    gem_pulseout_set_period(_working_buf[0], UNPACK_32(_working_buf, 1));
}

static void _cmd_0x07_erase_settings(uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_settings_erase();
}

static void _cmd_0x08_read_settings(uint8_t* data, size_t len) {
    /* Settings are sent in chunks to avoid overflowing midi buffers. */
    /* Request: CHUNK_NUM(1) */
    /* Response (teeth): SETTINGS_CHUNK(CHUNK_SIZE) */
    (void)(data);
    (void)(len);

    const uint8_t chunk_num = data[2];
    if (chunk_num >= TOTAL_CHUNKS) {
        printf("Invalid chunk %u.\r\n", chunk_num);
        return;
    }

    struct gem_settings settings;
    uint8_t _settings_buf[MAX_SETTINGS_SIZE];
    gem_settings_load(&settings);
    gem_settings_serialize(&settings, _settings_buf);

    teeth_encode(_settings_buf, MAX_SETTINGS_SIZE, _working_buf);

    gem_usb_midi_send((uint8_t[4]){MIDI_SYSEX_START_OR_CONTINUE, MIDI_SYSEX_START_BYTE, GEM_MIDI_SYSEX_MARKER, 0x08});
    gem_midi_send_sysex(_working_buf + (CHUNK_SIZE * chunk_num), CHUNK_SIZE);
}

static void _cmd_0x09_write_settings(uint8_t* data, size_t len) {
    /* Settings are sent in chunks to avoid overflowing midi buffers. */
    /* Request (teeth): CHUNK_NUM(1) SETTINGS_CHUNK(CHUNK_SIZE) */
    (void)(len);

    teeth_decode(data + 2, CHUNK_SIZE + 1, _working_buf);

    const uint8_t chunk_num = _working_buf[0];
    if (chunk_num >= TOTAL_CHUNKS) {
        printf("Invalid chunk %u.\r\n", chunk_num);
        return;
    }

    if (chunk_num == 0) {
        memset(_chunk_buf, 0xFF, TOTAL_BYTES);
    }

    memcpy(_chunk_buf + (CHUNK_SIZE * chunk_num), _working_buf, CHUNK_SIZE);

    /* All data received, save the settings. */
    if (chunk_num == TOTAL_CHUNKS - 1) {
        teeth_decode(_chunk_buf, TOTAL_BYTES, _working_buf);

        struct gem_settings settings;

        if (gem_settings_deserialize(&settings, _working_buf)) {
            gem_settings_save(&settings);
            printf("Saved settings: \r\n");
            gem_settings_print(&settings);
        } else {
            printf("Failed to save settings, unable to deserialize.\r\n");
        }
    }

    /* Ack the data. */
    gem_midi_send_sysex((uint8_t[2]){GEM_MIDI_SYSEX_MARKER, 0x09}, 2);
}

static void _cmd_0x0A_write_lut_entry(uint8_t* data, size_t len) {
    /* Request (teeth): ENTRY(1) OSC(1) CODE(4) */
    (void)(len);

    teeth_decode(data + 2, 6, _working_buf);

    size_t entry = _working_buf[0];
    uint8_t osc = _working_buf[1];
    uint16_t code = UNPACK_16(_working_buf, 2);

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
    gem_midi_send_sysex((uint8_t[2]){GEM_MIDI_SYSEX_MARKER, 0x0A}, 2);
}

static void _cmd_0x0B_write_lut(uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_save_dac_codes_table();
}

static void _cmd_0x0C_erase_lut(uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    gem_save_dac_codes_table();
}

static void _cmd_0x0D_disable_adc_corr(uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);
    gem_adc_set_error_correction(2048, 0);
}

static void _cmd_0x0E_enable_adc_corr(uint8_t* data, size_t len) {
    (void)(data);
    (void)(len);

    struct gem_settings settings;
    gem_settings_load(&settings);
    gem_adc_set_error_correction(settings.adc_gain_corr, settings.adc_offset_corr);
}