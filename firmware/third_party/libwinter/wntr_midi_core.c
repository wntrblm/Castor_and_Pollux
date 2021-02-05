/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_midi_core.h"
#include "class/midi/midi_device.h"
#include "printf.h"
#include <stdbool.h>
#include <stdint.h>

/* Macros & definitions */

#define SYSEX_BUF_SIZE 64
#define SYSEX_TIMEOUT 100000
#define SYSEX_START_BYTE 0xF0
#define SYSEX_END_BYTE 0xF7

/* Enums */

enum USBMIDICodeIndexes {
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

static uint8_t in_packet_[4];
static uint8_t sysex_data_[SYSEX_BUF_SIZE];
static size_t sysex_data_len_;
static wntr_midi_sysex_callback sysex_callback_;

/* Private forward declarations. */

static void consume_sysex();

/* Public functions. */

void wntr_midi_task() {
    if (tud_midi_receive(in_packet_) == false) {
        return;
    }

    if ((in_packet_[0] & 0x0F) == CIN_SYSEX_START_OR_CONTINUE) {
        consume_sysex();
        if (sysex_data_len_ > 0) {
            sysex_callback_(sysex_data_, sysex_data_len_);
        }
    }
}

static void consume_sysex() {
    /* Start message has 3 of the sysex bytes. */
    sysex_data_[0] = in_packet_[1];
    sysex_data_[1] = in_packet_[2];
    sysex_data_[2] = in_packet_[3];
    size_t data_index = 3;

    while (1) {

        /* Wait until we get a message, but fail out if it doesn't arrive in time. */
        size_t m = 0;
        for (; m < SYSEX_TIMEOUT; m++) {
            if (tud_midi_receive(in_packet_)) {
                break;
            }
        }

        if (m == SYSEX_TIMEOUT) {
            goto timeout_fail;
        }

        if ((in_packet_[0] & 0x0F) == CIN_SYSEX_START_OR_CONTINUE) {
            if (data_index + 3 > SYSEX_BUF_SIZE - 1)
                break;
            sysex_data_[data_index++] = in_packet_[1];
            sysex_data_[data_index++] = in_packet_[2];
            sysex_data_[data_index++] = in_packet_[3];
        } else if ((in_packet_[0] & 0x0F) == CIN_SYSEX_END_THREE_BYTE) {
            if (data_index + 3 > SYSEX_BUF_SIZE - 1)
                break;
            sysex_data_[data_index++] = in_packet_[1];
            sysex_data_[data_index++] = in_packet_[2];
            sysex_data_[data_index++] = in_packet_[3];
            break;
        } else if ((in_packet_[0] & 0x0F) == CIN_SYSEX_END_TWO_BYTE) {
            if (data_index + 2 > SYSEX_BUF_SIZE - 1)
                break;
            sysex_data_[data_index++] = in_packet_[1];
            sysex_data_[data_index++] = in_packet_[2];
            break;
        } else if ((in_packet_[0] & 0x0F) == CIN_SYSEX_END_ONE_BYTE) {
            if (data_index + 1 > SYSEX_BUF_SIZE - 1)
                break;
            sysex_data_[data_index++] = in_packet_[1];
            break;
        }
    }

    sysex_data_len_ = data_index;

    return;

timeout_fail:
    sysex_data_len_ = 0;
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
static bool sysex_iterator_next_(const uint8_t* data, size_t len, size_t* head, uint8_t* dst) {
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

static size_t sysex_iterator_remaining_(size_t len, size_t head) { return len + 2 - head; }

void wntr_midi_send_sysex(const uint8_t* data, size_t len) {
    size_t head = 0;
    while (true) {
        uint8_t packet[] = {0, 0, 0, 0};
        size_t remaining = sysex_iterator_remaining_(len, head);
        if (remaining > 3) {
            packet[0] = CIN_SYSEX_START_OR_CONTINUE;
            sysex_iterator_next_(data, len, &head, packet + 1);
            sysex_iterator_next_(data, len, &head, packet + 2);
            sysex_iterator_next_(data, len, &head, packet + 3);
            tud_midi_send(packet);
            continue;
        } else if (remaining == 3) {
            packet[0] = CIN_SYSEX_END_THREE_BYTE;
            sysex_iterator_next_(data, len, &head, packet + 1);
            sysex_iterator_next_(data, len, &head, packet + 2);
            sysex_iterator_next_(data, len, &head, packet + 3);
            tud_midi_send(packet);
            return;
        } else if (remaining == 2) {
            packet[0] = CIN_SYSEX_END_TWO_BYTE;
            sysex_iterator_next_(data, len, &head, packet + 1);
            sysex_iterator_next_(data, len, &head, packet + 2);
            tud_midi_send(packet);
            return;
        } else if (remaining == 1) {
            packet[0] = CIN_SYSEX_END_ONE_BYTE;
            sysex_iterator_next_(data, len, &head, packet + 1);
            tud_midi_send(packet);
            return;
        }
    };
}

void wntr_midi_set_sysex_callback(wntr_midi_sysex_callback callback) { sysex_callback_ = callback; }
