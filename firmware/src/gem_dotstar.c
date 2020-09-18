#include "gem_dotstar.h"
#include "gem_config.h"
#include "gem_spi.h"

static uint16_t _brightness = 127;
static uint8_t _pixels[GEM_DOTSTAR_COUNT * 3];
static const uint8_t _start_frame[4] = {0x00, 0x00, 0x00, 0x00};

void gem_dotstar_init() {
    // TODO: Set global brightness
}

void gem_dotstar_set(size_t n, uint8_t r, uint8_t g, uint8_t b) {
    _pixels[n * 3] = (r * _brightness) >> 8;
    _pixels[n * 3 + 1] = (g * _brightness) >> 8;
    _pixels[n * 3 + 2] = (b * _brightness) >> 8;
}

void gem_dotstar_update() {
    /* Write start frame. */
    gem_spi_write(_start_frame, 4);
    /* Write each pixel. */
    for (size_t i = 0; i < GEM_DOTSTAR_COUNT; i++) {
        gem_spi_write((uint8_t[]){0xFF}, 1);
        gem_spi_write(_pixels + (i * 3), 3);
    }
    /* Write end frame. */
    for (size_t i = 0; i < GEM_DOTSTAR_COUNT / 2 + 1; i++) { gem_spi_write((uint8_t[]){0xFF}, 1); }
}