/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_midi_core.h"
#include "gem_config.h"
#include "gem_usb.h"
#include "printf.h"
#include <stdbool.h>
#include <stdint.h>

/* Macros & definitions */

#define SYSEX_START_BYTE 0xF0
#define SYSEX_END_BYTE 0xF7

/* Enums */

enum GemUSBMIDICodeIndexes {
    CIN_TWO_BYTE_COMMON = 0x2,
    CIN_THREE_BYTE_COMMON = 0x3,
    CIN_SYSEX_START_OR_CONTINUE = 0x4,
    CIN_SYSEX_END_ONE_BYTE = 0x5,
    CIN_SYSEX_END_TWO_BYTE = 0x6,
    CIN_SYSEX_END_THREE_BYTE = 0x7,
    CIN_NOTE_OFF = 0x8,
    CIN_NOTE_ON = 0x9,
    CIN_POLY_KEY_PRESS = 0xA,
    CIN_CONTROL_CHANGE = 0xB,
    CIN_PROGRAM_CHANGE = 0xC,
    CIN_CHANNEL_PRESSURE = 0xD,
    CIN_PITCH_BEND = 0xE,
    CIN_UNPARSED_SINGLE_BYTE = 0xF
};

/* Static variables */

static uint8_t _in_data[4];
static uint8_t _sysex_data[GEM_SYSEX_BUF_SIZE];
static size_t _sysex_data_len;
static gem_midi_sysex_callback _sysex_callback;

/* Private forward declarations. */

static void _parse_sysex();

/* Public functions. */

void gem_midi_task() {
    if (gem_usb_midi_receive(_in_data) == false) {
        return;
    }

    if ((_in_data[0] & 0x0F) == CIN_SYSEX_START_OR_CONTINUE) {
        _parse_sysex();
        if (_sysex_data_len > 0) {
            _sysex_callback(_sysex_data, _sysex_data_len);
        }
    }
}

static void _parse_sysex() {
    /* Start message has 3 of the sysex bytes. */
    _sysex_data[0] = _in_data[1];
    _sysex_data[1] = _in_data[2];
    _sysex_data[2] = _in_data[3];
    size_t data_index = 3;

    while (1) {

        /* Wait until we get a message, but fail out if it doesn't arrive in time. */
        size_t m = 0;
        for (; m < GEM_SYSEX_TIMEOUT; m++) {
            if (gem_usb_midi_receive(_in_data)) {
                break;
            }
        }

        if (m == GEM_SYSEX_TIMEOUT) {
            goto timeout_fail;
        }

        if ((_in_data[0] & 0x0F) == CIN_SYSEX_START_OR_CONTINUE) {
            if (data_index + 3 > GEM_SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            _sysex_data[data_index++] = _in_data[3];
        } else if ((_in_data[0] & 0x0F) == CIN_SYSEX_END_THREE_BYTE) {
            if (data_index + 3 > GEM_SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            _sysex_data[data_index++] = _in_data[3];
            break;
        } else if ((_in_data[0] & 0x0F) == CIN_SYSEX_END_TWO_BYTE) {
            if (data_index + 2 > GEM_SYSEX_BUF_SIZE - 1)
                break;
            _sysex_data[data_index++] = _in_data[1];
            _sysex_data[data_index++] = _in_data[2];
            break;
        } else if ((_in_data[0] & 0x0F) == CIN_SYSEX_END_ONE_BYTE) {
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

/*
    This is an iterator that's used to wrap sysex messages with the MIDI SysEx
    start byte and end byte without having to allocate more memory.

    It's roughly equivalent to this Python generator:

    def sysex_generator(data):
        yield MIDI_SYSEX_START_BYTE
        for b in data:
            yield b
        yield MIDI_SYSEX_START_BYTE
*/
static bool sysex_iterator_next(const uint8_t* data, size_t len, size_t* head, uint8_t* dst) {
    if (*head == 0) {
        (*dst) = (uint8_t)SYSEX_START_BYTE;
        (*head)++;
        return true;
    }
    if (*head == len + 1) {
        (*dst) = (uint8_t)SYSEX_END_BYTE;
        (*head)++;
        return true;
    }
    if (*head > len + 1) {
        return false;
    }
    (*dst) = data[(*head) - 1];
    (*head)++;
    return true;
}

static size_t sysex_iterator_remaining(size_t len, size_t head) { return len + 2 - head; }

void gem_midi_send_sysex(const uint8_t* data, size_t len) {
    size_t head = 0;
    while (true) {
        uint8_t packet[] = {0, 0, 0, 0};
        size_t remaining = sysex_iterator_remaining(len, head);
        if (remaining > 3) {
            packet[0] = CIN_SYSEX_START_OR_CONTINUE;
            sysex_iterator_next(data, len, &head, packet + 1);
            sysex_iterator_next(data, len, &head, packet + 2);
            sysex_iterator_next(data, len, &head, packet + 3);
            gem_usb_midi_send(packet);
            continue;
        } else if (remaining == 3) {
            packet[0] = CIN_SYSEX_END_THREE_BYTE;
            sysex_iterator_next(data, len, &head, packet + 1);
            sysex_iterator_next(data, len, &head, packet + 2);
            sysex_iterator_next(data, len, &head, packet + 3);
            gem_usb_midi_send(packet);
            return;
        } else if (remaining == 2) {
            packet[0] = CIN_SYSEX_END_TWO_BYTE;
            sysex_iterator_next(data, len, &head, packet + 1);
            sysex_iterator_next(data, len, &head, packet + 2);
            gem_usb_midi_send(packet);
            return;
        } else if (remaining == 1) {
            packet[0] = CIN_SYSEX_END_ONE_BYTE;
            sysex_iterator_next(data, len, &head, packet + 1);
            gem_usb_midi_send(packet);
            return;
        }
    };
}

void gem_midi_set_sysex_callback(gem_midi_sysex_callback callback) { _sysex_callback = callback; }
