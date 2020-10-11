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

static gem_midi_sysex_callback _sysex_callback;
static uint8_t _in_data[4];
static uint8_t _sysex_data[GEM_SYSEX_BUF_SIZE];

/* Private forward declarations. */

void _parse_sysex();
void _process_sysex_command();

/* Public functions. */

void gem_midi_set_sysex_callback(gem_midi_sysex_callback callback) { _sysex_callback = callback; }

void gem_midi_task() {
    if (gem_usb_midi_receive(_in_data) == false) {
        return;
    }

    if ((_in_data[0] & 0x0F) == MIDI_SYSEX_START_OR_CONTINUE) {
        _parse_sysex();
        if (_sysex_callback != NULL)
            _sysex_callback(_sysex_data);
    }
}

void _parse_sysex() {
    /* Take the last two bytes of the start message. */
    _sysex_data[0] = _in_data[2];
    _sysex_data[1] = _in_data[3];
    size_t data_index = 2;

    /* TODO: Consider a timeout here. */
    while (1) {
        /* Wait until we get a message. */
        while (!gem_usb_midi_receive(_in_data)) {};

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

void gem_midi_encode(uint8_t* src, uint8_t* dst, size_t src_len) {
    /* We encode middle data as one nibble per byte, dst must be twice the length of src. */
    for (size_t i = 0; i < src_len; i++) {
        dst[i * 2] = src[i] >> 4 & 0xF;
        dst[i * 2 + 1] = src[i] & 0xF;
    }
}

void gem_midi_decode(uint8_t* src, uint8_t* dst, size_t dst_len) {
    /* We encode middle data as one nibble per byte, so dst should be half the length of src. */
    for (size_t i = 0; i < dst_len; i += 1) { dst[i] = src[i * 2] << 4 | src[i * 2 + 1]; }
}