/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "printf.h"
#include "tusb.h"
#include "wntr_serial_number.h"

tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    // Allocated by Adafruit for Winterbloom
    // https://github.com/adafruit/uf2-samdx1/issues/136
    .idVendor = 0x239A,
    .idProduct = 0x80C3,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01};

enum { ITF_NUM_MIDI = 0, ITF_NUM_MIDI_STREAMING, ITF_NUM_TOTAL };

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN)
#define EPNUM_MIDI 0x01

uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 0, 0x02, 0x81, 64)};

const char* string_desc_arr[] = {
    (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "Winterbloom",               // 1: Manufacturer
    "Gemini",                    // 2: Product
};

static char serial_number_str_[] = "00000000FFFFFFFF";  // 3: Serial number
static uint16_t desc_str_[32];

/* Callback functions. */

uint8_t const* tud_descriptor_device_cb(void) { return (uint8_t const*)&desc_device; }

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;  // for multiple configurations
    return desc_configuration;
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    uint8_t chr_count;

    if (index == 0) {
        memcpy(&desc_str_[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else if (index == 3) {
        // Serial number.
        uint8_t serial_number[WNTR_SERIAL_NUMBER_LEN];
        wntr_serial_number(serial_number);
        snprintf(
            serial_number_str_,
            16 + 1,
            "%2x%2x%2x%2x%2x%2x%2x%2x",
            serial_number[0],
            serial_number[1],
            serial_number[2],
            serial_number[3],
            serial_number[4],
            serial_number[5],
            serial_number[6],
            serial_number[7]);
        for (uint8_t i = 0; i < 16; i++) { desc_str_[1 + i] = serial_number_str_[i]; }
        chr_count = 16;
    } else {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
            return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if (chr_count > 31)
            chr_count = 31;

        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++) { desc_str_[1 + i] = str[i]; }
    }

    // first byte is length (including header), second byte is string type
    desc_str_[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return desc_str_;
}
