#include "gem_midi_sysex.h"
#include "gem_adc.h"
#include "gem_config.h"
#include "gem_mcp4728.h"
#include "gem_midi_core.h"
#include "gem_pulseout.h"
#include "gem_settings.h"
#include "gem_usb.h"
#include "printf.h"
#include <string.h>

/* Defines & enums */

#define SYSEX_CMD_MARKER 0x77

enum sysex_commands {
    CMD_HELLO = 0x01,
    CMD_WRITE_ADC_GAIN = 0x02,
    CMD_WRITE_ADC_OFFSET = 0x03,
    CMD_READ_SETTINGS = 0x04,
    CMD_WRITE_SETTINGS = 0x05,
    CMD_RESET_SETTINGS = 0x06,
    CMD_READ_ADC = 0xA5,
    CMD_SET_DAC = 0xA6,
    CMD_SET_FREQ = 0xA7,
};

/* Static variables. */

static gem_sysex_event_callback _event_callback;
static uint8_t _settings_buf[64];
static uint8_t _encoding_buf[128];

void gem_set_sysex_event_callback(gem_sysex_event_callback callback) { _event_callback = callback; }

void gem_process_sysex(uint8_t* data) {
    /* Print out the bytes. */
    printf("Sysex Message: ");
    for (size_t i = 0; i < 16; i++) { printf("%02x ", data[i]); }
    printf("\r\n");

    if (data[0] != SYSEX_CMD_MARKER) {
        printf("Sysex message contains invalid marker: %02x\r\n", data[0]);
        return;
    }

    printf("Recieved sysex message: %02x\r\n", data[1]);

    struct gem_settings settings;
    gem_settings_load(&settings);

    switch (data[1]) {
        case CMD_HELLO:
            if (_event_callback != NULL) {
                _event_callback(GEM_SYSEX_EVENT_CALIBRATION_MODE);
            }
            gem_usb_midi_send(
                (uint8_t[4]){MIDI_SYSEX_START_OR_CONTINUE, MIDI_SYSEX_START_BYTE, SYSEX_CMD_MARKER, CMD_HELLO});
            gem_usb_midi_send((uint8_t[4]){MIDI_SYSEX_END_TWO_BYTE, GEM_FIRMWARE_VERSION, MIDI_SYSEX_END_BYTE, 0x00});
            break;

        case CMD_WRITE_ADC_GAIN:
            settings.adc_gain_corr = data[2] << 12 | data[3] << 8 | data[4] << 4 | data[5];
            gem_settings_save(&settings);
            break;

        case CMD_WRITE_ADC_OFFSET:
            settings.adc_offset_corr = data[2] << 12 | data[3] << 8 | data[4] << 4 | data[5];
            gem_settings_save(&settings);
            break;

        case CMD_READ_ADC: {
            uint16_t result = gem_adc_read_sync(&gem_adc_inputs[data[2]]);
            gem_usb_midi_send(
                (uint8_t[4]){MIDI_SYSEX_START_OR_CONTINUE, MIDI_SYSEX_START_BYTE, SYSEX_CMD_MARKER, CMD_READ_ADC});
            gem_usb_midi_send((uint8_t[4]){
                MIDI_SYSEX_START_OR_CONTINUE, (result >> 12) & 0xF, (result >> 8) & 0xF, (result >> 4) & 0xF});
            gem_usb_midi_send((uint8_t[4]){MIDI_SYSEX_END_TWO_BYTE, result & 0xF, MIDI_SYSEX_END_BYTE, 0x00});
        } break;

        case CMD_SET_DAC: {
            struct gem_mcp4728_channel dac_settings;
            dac_settings.vref = data[3];
            dac_settings.value = data[4] << 12 | data[5] << 8 | data[6] << 4 | data[7];
            gem_mcp_4728_write_channel(data[2], dac_settings);
        } break;

        case CMD_SET_FREQ:
            gem_pulseout_set_period(data[2], data[3] << 12 | data[4] << 8 | data[5] << 4 | data[6]);
            break;

        case CMD_RESET_SETTINGS:
            gem_settings_erase();
            break;

        case CMD_READ_SETTINGS: {
            gem_settings_serialize(&settings, _settings_buf);
            gem_midi_encode(_settings_buf, _encoding_buf, 64);
            gem_usb_midi_send(
                (uint8_t[4]){MIDI_SYSEX_START_OR_CONTINUE, MIDI_SYSEX_START_BYTE, SYSEX_CMD_MARKER, CMD_READ_SETTINGS});
            /* Settings are sent in 16 byte chunks to avoid overflowing midi buffers. */
            if (data[2] < 8) {
                gem_midi_send_sysex(_encoding_buf + (16 * data[2]), 16);
            }
        } break;

        case CMD_WRITE_SETTINGS: {
            /* Settings are sent in 16 byte chunks to avoid overflowing midi buffers. */
            if (data[2] == 0) {
                memset(_encoding_buf, 0xFF, 128);
            }
            if (data[2] < 8) {
                memcpy(_encoding_buf + (16 * data[2]), data + 3, 16);
                /* Ack the data. */
                gem_usb_midi_send((uint8_t[4]){
                    MIDI_SYSEX_START_OR_CONTINUE, MIDI_SYSEX_START_BYTE, SYSEX_CMD_MARKER, CMD_WRITE_SETTINGS});
                gem_usb_midi_send((uint8_t[4]){MIDI_SYSEX_END_ONE_BYTE, MIDI_SYSEX_END_BYTE, 0x00, 0x00});
            }
            if (data[2] == 7) {
                /* All data recieved, save the settings. */
                gem_midi_decode(_encoding_buf, _settings_buf, 64);
                if (gem_settings_deserialize(&settings, _settings_buf)) {
                    gem_settings_save(&settings);
                }
            }
        } break;

        default:
            break;
    }
}