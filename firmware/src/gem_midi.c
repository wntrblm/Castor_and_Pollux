#include "gem_midi.h"
#include "gem_config.h"
#include "gem_usb.h"
#include "gem_mcp4728.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define SYSEX_BUF_SIZE 16
#define SYSEX_CMD_MARKER 0x77
#define SYSEX_START_OR_CONTINUE 0x04
#define SYSEX_END_THREE_BYTE 0x07
#define SYSEX_END_TWO_BYTE 0x06
#define SYSEX_END_ONE_BYTE 0x05

enum sysex_commands {
    SE_CMD_HELLO = 0x01,
    SE_CMD_WRITE_ADC_GAIN = 0x02,
    SE_CMD_WRITE_ADC_OFFSET = 0x03,
    SE_CMD_WRITE_LED_BRIGHTNESS = 0x04,
};

static uint8_t _in_data[4];
static uint8_t _sysex_data[SYSEX_BUF_SIZE];

/* Private forward declarations. */

void _parse_sysex();
void _process_sysex_command();

/* Public functions. */

void gem_midi_task() {
    if (gem_usb_midi_receive(_in_data) == false) {
        return;
    }

    printf("MIDI IN: %02x %02x %02x %02x \r\n", _in_data[0], _in_data[1], _in_data[2], _in_data[3]);

    if ((_in_data[0] & 0x0F) == SYSEX_START_OR_CONTINUE) {
        _parse_sysex();
        _process_sysex_command();
    }
}

void _parse_sysex() {
    /* Take the last two bytes of the start message. */
    _sysex_data[0] = _in_data[2];
    _sysex_data[1] = _in_data[3];
    size_t data_index = 2;

    while (1) {
        /* Wait until we get a message. */
        while (!gem_usb_midi_receive(_in_data))
            ;

        if ((_in_data[0] & 0x0F) == SYSEX_START_OR_CONTINUE) {
            if (data_index + 3 > SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            _sysex_data[data_index++] = _in_data[3];
        } else if ((_in_data[0] & 0x0F) == SYSEX_END_THREE_BYTE) {
            if (data_index + 3 > SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            _sysex_data[data_index++] = _in_data[3];
            break;
        } else if ((_in_data[0] & 0x0F) == SYSEX_END_TWO_BYTE) {
            if (data_index + 2 > SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            break;
        } else if ((_in_data[0] & 0x0F) == SYSEX_END_ONE_BYTE) {
            if (data_index + 1 > SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            break;
        }
    }
}

void _process_sysex_command() {
    /* Print out the bytes. */
    __wrap_printf("Sysex Message: ");
    for (size_t i = 0; i < 16; i++) { printf("%02x ", _sysex_data[i]); }
    __wrap_printf("\r\n");

    if(_sysex_data[0] != SYSEX_CMD_MARKER) {
        return;
    }

    struct gem_nvm_settings settings;

    switch(_sysex_data[1]) {
        case SE_CMD_HELLO:
            /* Set DAC test. */
            gem_mcp_4728_write_channel(_sysex_data[2], _sysex_data[3] << 8 | _sysex_data[4] << 4 | _sysex_data[5]);
            break;

        case SE_CMD_WRITE_ADC_GAIN:
            gem_config_get_nvm_settings(&settings);
            settings.adc_gain_corr = _sysex_data[2] << 12 | _sysex_data[3] << 8 | _sysex_data[4] << 4 | _sysex_data[5];
            gem_config_save_nvm_settings(&settings);
            break;

        case SE_CMD_WRITE_ADC_OFFSET:
            gem_config_get_nvm_settings(&settings);
            settings.adc_offset_corr = _sysex_data[2] << 12 | _sysex_data[3] << 8 | _sysex_data[4] << 4 | _sysex_data[5];
            gem_config_save_nvm_settings(&settings);
            break;

        case SE_CMD_WRITE_LED_BRIGHTNESS:
            gem_config_get_nvm_settings(&settings);
            settings.led_brightness = _sysex_data[2] << 12 | _sysex_data[3] << 8 | _sysex_data[4] << 4 | _sysex_data[5];
            gem_config_save_nvm_settings(&settings);
            break;

        default:
            break;
    }
}