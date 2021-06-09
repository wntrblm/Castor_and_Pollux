/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_midi_core.h"
#include "class/midi/midi_device.h"
#include "printf.h"
#include "wntr_ticks.h"
#include <stdbool.h>
#include <stdint.h>

/* Macros & definitions */

#define SYSEX_BUF_SIZE 128
#define SYSEX_TIMEOUT 100 /* Milliseconds */
#define SYSEX_START_BYTE 0xF0
#define SYSEX_END_BYTE 0xF7

/* Static variables */

static uint8_t sysex_data_[SYSEX_BUF_SIZE];
static size_t sysex_data_len_;

/* Private forward declarations. */

static bool midi_read(struct WntrMIDIMessage* msg);
static void consume_sysex();
static bool sysex_iterator_next(const uint8_t* data, size_t len, size_t* head, uint8_t* dst);
static size_t sysex_iterator_remaining(size_t len, size_t head);

/* Public functions. */

bool wntr_midi_receive(struct WntrMIDIMessage* msg) {
    if (midi_read(msg) == false) {
        return false;
    }

    if (msg->code_index == MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE) {
        consume_sysex(msg);
        msg->code_index = MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE;
        msg->status = 0;
        msg->data_0 = 0;
        msg->data_1 = 0;
    }

    return true;
}

void wntr_midi_send(const struct WntrMIDIMessage* msg) {
    uint8_t packet[] = {0, 0, 0, 0};
    packet[0] = msg->cable << 4 | msg->code_index;
    packet[1] = msg->status;
    packet[2] = msg->data_0;
    packet[3] = msg->data_1;
    tud_midi_send(packet);
}

size_t wntr_midi_sysex_len() { return sysex_data_len_; }

const uint8_t* wntr_midi_sysex_data() { return sysex_data_; }

void wntr_midi_send_sysex(const uint8_t* data, size_t len) {
    size_t head = 0;
    while (true) {
        uint8_t packet[] = {0, 0, 0, 0};
        size_t remaining = sysex_iterator_remaining(len, head);
        if (remaining > 3) {
            packet[0] = MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE;
            sysex_iterator_next(data, len, &head, packet + 1);
            sysex_iterator_next(data, len, &head, packet + 2);
            sysex_iterator_next(data, len, &head, packet + 3);
            tud_midi_send(packet);
            continue;
        } else if (remaining == 3) {
            packet[0] = MIDI_CODE_INDEX_SYSEX_END_THREE_BYTE;
            sysex_iterator_next(data, len, &head, packet + 1);
            sysex_iterator_next(data, len, &head, packet + 2);
            sysex_iterator_next(data, len, &head, packet + 3);
            tud_midi_send(packet);
            return;
        } else if (remaining == 2) {
            packet[0] = MIDI_CODE_INDEX_SYSEX_END_TWO_BYTE;
            sysex_iterator_next(data, len, &head, packet + 1);
            sysex_iterator_next(data, len, &head, packet + 2);
            tud_midi_send(packet);
            return;
        } else if (remaining == 1) {
            packet[0] = MIDI_CODE_INDEX_SYSEX_END_ONE_BYTE;
            sysex_iterator_next(data, len, &head, packet + 1);
            tud_midi_send(packet);
            return;
        }
    };
}

/* Private functions */

static bool midi_read(struct WntrMIDIMessage* msg) {
    uint8_t packet[4];
    if (!tud_midi_receive(packet)) {
        return false;
    }

    msg->cable = packet[0] >> 4 & 0xF;
    msg->code_index = packet[0] & 0xF;
    msg->status = packet[1];
    msg->data_0 = packet[2];
    msg->data_1 = packet[3];

    return true;
};

static void consume_sysex(struct WntrMIDIMessage* msg) {
    /* Start message has 3 of the sysex bytes, skip the first (0xF0). */
    sysex_data_[0] = msg->data_0;
    sysex_data_[1] = msg->data_1;
    size_t data_index = 2;

    while (true) {
        /* Wait until we get a message, but fail out if it doesn't arrive in time. */
        uint32_t start_ticks = wntr_ticks();
        bool timeout = true;
        while (wntr_ticks() < start_ticks + SYSEX_TIMEOUT) {
            /*
                Call tud_task() to keep USB data flowing, otherwise, this
                won't recieve any data that isn't already in the USB buffer
                before the loop started.
            */
            tud_task();
            if (midi_read(msg)) {
                timeout = false;
                break;
            }
        }

        if (timeout) {
            goto timeout_fail;
        }

        switch (msg->code_index) {
            case MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE:
                sysex_data_[data_index++] = msg->status;
                sysex_data_[data_index++] = msg->data_0;
                sysex_data_[data_index++] = msg->data_1;
                break;

            case MIDI_CODE_INDEX_SYSEX_END_THREE_BYTE:
                sysex_data_[data_index++] = msg->status;
                sysex_data_[data_index++] = msg->data_0;
                /* Ignore last byte (0xF7) */
                goto exit;

            case MIDI_CODE_INDEX_SYSEX_END_TWO_BYTE:
                sysex_data_[data_index++] = msg->status;
                /* Ignore last byte (0xF7) */
                goto exit;

            case MIDI_CODE_INDEX_SYSEX_END_ONE_BYTE:
                /* Ignore last byte (0xF7) */
                goto exit;

            default:
                goto exit;
        }
    }

exit:
    sysex_data_len_ = data_index;
    return;

timeout_fail:
    printf("Timed out while waiting for SysEx.\n");
    sysex_data_len_ = 0;
    printf("Received %u bytes before timeout:\n", data_index);
    for (size_t i = 0; i < data_index; i++) {
        printf("0x%02x ", sysex_data_[i]);
        if (i > 16) {
            printf("... (%u more)", data_index - i);
            break;
        }
    }
    printf("\n");
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
