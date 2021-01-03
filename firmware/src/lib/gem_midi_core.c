#include "gem_midi_core.h"
#include "class/midi/midi_device.h"
#include "gem_adc.h"
#include "gem_config.h"
#include "gem_mcp4728.h"
#include "gem_pulseout.h"
#include "gem_usb.h"
#include "printf.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_SYSEX_CALLBACKS 16

static gem_midi_sysex_command_callback _sysex_callbacks[MAX_SYSEX_CALLBACKS];
static uint8_t _in_data[4];
static uint8_t _sysex_data[GEM_SYSEX_BUF_SIZE];
static size_t _sysex_data_len;

/* Private forward declarations. */

static void _parse_sysex();

/* Public functions. */

void gem_midi_task() {
    if (gem_usb_midi_receive(_in_data) == false) {
        return;
    }

    if ((_in_data[0] & 0x0F) == MIDI_SYSEX_START_OR_CONTINUE) {
        _parse_sysex();
        if (_sysex_data_len < 2) {
            printf("Invalid SysEx (too short): %02x, length: %lu\r\n", _sysex_data[0], _sysex_data_len);
            return;
        }
        if (_sysex_data[0] != GEM_MIDI_SYSEX_MARKER) {
            printf("Invalid SysEx (wrong marker byte): %02x, length: %lu\r\n", _sysex_data[1], _sysex_data_len);
            return;
        }

        uint8_t command = _sysex_data[1];
        if (command >= MAX_SYSEX_CALLBACKS || _sysex_callbacks[command] == NULL) {
            printf("Invalid SysEx (invalid command): %02x, length: %lu\r\n", command, _sysex_data_len);
            return;
        }
        _sysex_callbacks[command](_sysex_data, _sysex_data_len);
    }
}

static void _parse_sysex() {
    /* Take the last two bytes of the start message. */
    _sysex_data[0] = _in_data[2];
    _sysex_data[1] = _in_data[3];
    size_t data_index = 2;

    while (1) {
    
        /* Wait until we get a message, but fail out if it doesn't arrive in time. */
        size_t m = 0;
        for(; m < GEM_SYSEX_TIMEOUT; m++){
            if(gem_usb_midi_receive(_in_data)) {
                break;
            }
        }

        if (m == GEM_SYSEX_TIMEOUT) {
            goto timeout_fail;
        }

        if ((_in_data[0] & 0x0F) == MIDI_SYSEX_START_OR_CONTINUE) {
            if (data_index + 3 > GEM_SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            _sysex_data[data_index++] = _in_data[3];
        } else if ((_in_data[0] & 0x0F) == MIDI_SYSEX_END_THREE_BYTE) {
            if (data_index + 3 > GEM_SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            _sysex_data[data_index++] = _in_data[3];
            break;
        } else if ((_in_data[0] & 0x0F) == MIDI_SYSEX_END_TWO_BYTE) {
            if (data_index + 2 > GEM_SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            break;
        } else if ((_in_data[0] & 0x0F) == MIDI_SYSEX_END_ONE_BYTE) {
            if (data_index + 1 > GEM_SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            break;
        }
    }

    _sysex_data_len = data_index;

    return;

timeout_fail:
    _sysex_data_len = 0;
    printf("Timed out while waiting for SysEx.");
    return;
}

void gem_midi_send_sysex(uint8_t* data, size_t len) {
    size_t i = 0;
    for (; i <= len - 3; i += 3) {
        gem_usb_midi_send(
            (uint8_t[4]){MIDI_SYSEX_START_OR_CONTINUE, data[i] & 0x7F, data[i + 1] & 0x7F, data[i + 2] & 0x7F});
    }
    if (len - i == 0) {
        gem_usb_midi_send((uint8_t[4]){MIDI_SYSEX_END_ONE_BYTE, MIDI_SYSEX_END_BYTE, 0x00, 0x00});
    }
    if (len - i == 1) {
        gem_usb_midi_send((uint8_t[4]){MIDI_SYSEX_END_TWO_BYTE, data[i] & 0x7F, MIDI_SYSEX_END_BYTE, 0x00});
    }
    if (len - i == 2) {
        gem_usb_midi_send(
            (uint8_t[4]){MIDI_SYSEX_END_THREE_BYTE, data[i] & 0x7F, data[i + 1] & 0x7F, MIDI_SYSEX_END_BYTE});
    }
}

void gem_midi_register_sysex_command(uint8_t command, gem_midi_sysex_command_callback callback) {
    _sysex_callbacks[command] = callback;
}