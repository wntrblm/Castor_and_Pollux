/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* Tests for src/lib/gem_midi_core.c */

#include "gem_sysex_dispatcher.h"
#include "gem_test.h"
#include "wntr_midi_core.h"

static uint8_t usb_midi_in_packets[1024];
static size_t usb_midi_in_packets_idx = 0;
static uint8_t usb_midi_out_packets[1024];
static size_t usb_midi_out_packets_idx = 0;
static uint8_t captured_sysex[128];
static size_t captured_sysex_len = 0;

bool tud_midi_n_receive(uint8_t idx, uint8_t packet[4]) {
    (void)(idx);

    if (usb_midi_in_packets[usb_midi_in_packets_idx] == 0) {
        return false;
    }
    packet[0] = usb_midi_in_packets[usb_midi_in_packets_idx];
    packet[1] = usb_midi_in_packets[usb_midi_in_packets_idx + 1];
    packet[2] = usb_midi_in_packets[usb_midi_in_packets_idx + 2];
    packet[3] = usb_midi_in_packets[usb_midi_in_packets_idx + 3];
    usb_midi_in_packets_idx += 4;
    fprintf(stderr, "Yielded packet\n");
    return true;
}

bool tud_midi_n_send(uint8_t idx, const uint8_t packet[4]) {
    (void)(idx);
    usb_midi_out_packets[usb_midi_out_packets_idx] = packet[0];
    usb_midi_out_packets[usb_midi_out_packets_idx + 1] = packet[1];
    usb_midi_out_packets[usb_midi_out_packets_idx + 2] = packet[2];
    usb_midi_out_packets[usb_midi_out_packets_idx + 3] = packet[3];
    usb_midi_out_packets_idx += 4;
    return true;
}

void reset_midi() {
    usb_midi_out_packets_idx = 0;
    memset(usb_midi_out_packets, 0, ARRAY_LEN(usb_midi_out_packets));
    usb_midi_in_packets_idx = 0;
    memset(usb_midi_in_packets, 0, ARRAY_LEN(usb_midi_in_packets));
    captured_sysex_len = 0;
    memset(captured_sysex, 0, ARRAY_LEN(captured_sysex));
}

static void sysex_callback(const uint8_t* data, size_t len) {
    memcpy(captured_sysex, data, len);
    captured_sysex_len = len;
}

TEST_CASE_BEGIN(send_sysex_one)
    /* one byte -> single packet */
    wntr_midi_send_sysex((const uint8_t[]){1}, 1);

    const uint8_t expected[] = {0x7, 0xF0, 1, 0xF7};
    print_hex(usb_midi_out_packets, ARRAY_LEN(expected));
    munit_assert_memory_equal(ARRAY_LEN(expected), usb_midi_out_packets, expected);
TEST_CASE_END

TEST_CASE_BEGIN(send_sysex_two)
    /* two bytes -> two packets */
    wntr_midi_send_sysex((const uint8_t[]){1, 2}, 2);

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
    wntr_midi_send_sysex((const uint8_t[]){1, 2, 3}, 3);

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
    wntr_midi_send_sysex((const uint8_t[]){1, 2, 3, 4}, 4);

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
    wntr_midi_send_sysex((const uint8_t[]){1, 2, 3, 4, 5, 6, 7, 8, 9}, 9);

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

TEST_CASE_BEGIN(receive_sysex_simple)
    // clang-format off
    const uint8_t midi_packets[] = {
        0x4, 0xF0, 0x77, 1,
        0x4, 2, 3, 4,
        0x4, 5, 6, 7,
        0x6, 8, 0xF7, 0
    };
    // clang-format on

    memcpy(usb_midi_in_packets, midi_packets, ARRAY_LEN(midi_packets));

    gem_sysex_register_command(0x1, sysex_callback);
    wntr_midi_set_sysex_callback(gem_sysex_dispatcher);

    wntr_midi_task();

    /* Command callback should be invoked with just the data payload. */
    const uint8_t expected[] = {2, 3, 4, 5, 6, 7, 8};

    munit_assert_size(captured_sysex_len, ==, ARRAY_LEN(expected));
    munit_assert_memory_equal(ARRAY_LEN(expected), captured_sysex, expected);
TEST_CASE_END

static void* setup_midi_tests(const MunitParameter params[], void* user_data) {
    (void)(params);
    (void)(user_data);
    reset_midi();
    return NULL;
}

static MunitTest test_suite_tests[] = {
    {.name = "send one byte sysex", .test = test_send_sysex_one, .setup = setup_midi_tests},
    {.name = "send two byte sysex", .test = test_send_sysex_two, .setup = setup_midi_tests},
    {.name = "send three byte sysex", .test = test_send_sysex_three, .setup = setup_midi_tests},
    {.name = "send four byte sysex", .test = test_send_sysex_four, .setup = setup_midi_tests},
    {.name = "send multibyte sysex", .test = test_send_sysex_many, .setup = setup_midi_tests},
    {.name = "receive simple sysex", .test = test_receive_sysex_simple, .setup = setup_midi_tests},
    {.test = NULL},
};

MunitSuite test_midi_core_suite = {
    .prefix = "midi core: ",
    .tests = test_suite_tests,
    .iterations = 1,
};
