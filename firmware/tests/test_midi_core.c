/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* Tests for src/lib/gem_midi_core.c */

#include "gem_midi_core.h"
#include "gem_test.h"

static uint8_t usb_midi_in_packets[1024];
static uint8_t usb_midi_in_packets_idx = 0;
static uint8_t usb_midi_out_packets[1024];
static uint8_t usb_midi_out_packets_idx = 0;

bool gem_usb_midi_receive(uint8_t packet[4]) {
    if (usb_midi_in_packets_idx == 0) {
        return false;
    }
    usb_midi_in_packets_idx -= 4;
    packet[0] = usb_midi_in_packets[usb_midi_in_packets_idx];
    packet[1] = usb_midi_in_packets[usb_midi_in_packets_idx + 1];
    packet[2] = usb_midi_in_packets[usb_midi_in_packets_idx + 2];
    packet[3] = usb_midi_in_packets[usb_midi_in_packets_idx + 3];
    return true;
}

bool gem_usb_midi_send(const uint8_t packet[4]) {
    usb_midi_out_packets[usb_midi_out_packets_idx] = packet[0];
    usb_midi_out_packets[usb_midi_out_packets_idx + 1] = packet[1];
    usb_midi_out_packets[usb_midi_out_packets_idx + 2] = packet[2];
    usb_midi_out_packets[usb_midi_out_packets_idx + 3] = packet[3];
    usb_midi_out_packets_idx += 4;
    return true;
}

void reset_midi_output() {
    usb_midi_out_packets_idx = 0;
    memset(usb_midi_out_packets, 0, 1024);
}

TEST_CASE_BEGIN(send_sysex_one)
    /* one byte -> single packet */
    gem_midi_send_sysex((const uint8_t[]){1}, 1);

    const uint8_t expected[] = {0x7, 0xF0, 1, 0xF7};
    print_hex(usb_midi_out_packets, ARRAY_LEN(expected));
    munit_assert_memory_equal(ARRAY_LEN(expected), usb_midi_out_packets, expected);

    reset_midi_output();
TEST_CASE_END

TEST_CASE_BEGIN(send_sysex_two)
    /* two bytes -> two packets */
    gem_midi_send_sysex((const uint8_t[]){1, 2}, 2);

    // clang-format off
    const uint8_t expected[] = {
        0x4, 0xF0, 1, 2,
        0x5, 0xF7, 0, 0
    };
    // clang-format on

    print_hex(usb_midi_out_packets, ARRAY_LEN(expected));
    munit_assert_memory_equal(ARRAY_LEN(expected), usb_midi_out_packets, expected);
TEST_CASE_END

TEST_CASE_BEGIN(send_sysex_three)
    /* three bytes -> two packets */
    gem_midi_send_sysex((const uint8_t[]){1, 2, 3}, 3);

    // clang-format off
    const uint8_t expected[] = {
        0x4, 0xF0, 1, 2,
        0x6, 3, 0xF7, 0,
    };
    // clang-format on

    print_hex(usb_midi_out_packets, ARRAY_LEN(expected));
    munit_assert_memory_equal(ARRAY_LEN(expected), usb_midi_out_packets, expected);
TEST_CASE_END

TEST_CASE_BEGIN(send_sysex_four)
    /* four bytes -> two packets */
    gem_midi_send_sysex((const uint8_t[]){1, 2, 3, 4}, 4);

    // clang-format off
    const uint8_t expected[] = {
        0x4, 0xF0, 1, 2,
        0x7, 3, 4, 0xF7,
    };
    // clang-format on

    print_hex(usb_midi_out_packets, ARRAY_LEN(expected));
    munit_assert_memory_equal(ARRAY_LEN(expected), usb_midi_out_packets, expected);
TEST_CASE_END

TEST_CASE_BEGIN(send_sysex_many)
    /* many bytes -> many packets */
    gem_midi_send_sysex((const uint8_t[]){1, 2, 3, 4, 5, 6, 7, 8, 9}, 9);

    // clang-format off
    const uint8_t expected[] = {
        0x4, 0xF0, 1, 2,
        0x4, 3, 4, 5,
        0x4, 6, 7, 8,
        0x6, 9, 0xF7, 0
    };
    // clang-format on

    print_hex(usb_midi_out_packets, ARRAY_LEN(expected));
    munit_assert_memory_equal(ARRAY_LEN(expected), usb_midi_out_packets, expected);
TEST_CASE_END

static void* setup_midi_tests(const MunitParameter params[], void* user_data) {
    (void)(params);
    (void)(user_data);
    reset_midi_output();
    return NULL;
}

static MunitTest test_suite_tests[] = {
    {.name = "send one byte sysex", .test = test_send_sysex_one, .setup = setup_midi_tests},
    {.name = "send two byte sysex", .test = test_send_sysex_two, .setup = setup_midi_tests},
    {.name = "send three byte sysex", .test = test_send_sysex_three, .setup = setup_midi_tests},
    {.name = "send four byte sysex", .test = test_send_sysex_four, .setup = setup_midi_tests},
    {.name = "send multibyte sysex", .test = test_send_sysex_many, .setup = setup_midi_tests},
    {.test = NULL},
};

MunitSuite test_midi_core_suite = {
    .prefix = "midi core: ",
    .tests = test_suite_tests,
    .iterations = 1,
};
