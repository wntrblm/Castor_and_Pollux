/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_dotstar.h"
#include "gem_config.h"
#include "gem_spi.h"

static uint16_t brightness_ = 0;
static uint8_t pixels_[GEM_MAX_DOTSTAR_COUNT * 3];
static const uint8_t start_frame_[4] = {0x00, 0x00, 0x00, 0x00};

void gem_dotstar_init(uint8_t brightness) { brightness_ = brightness; }

void gem_dotstar_set(size_t n, uint8_t r, uint8_t g, uint8_t b) {
    pixels_[n * 3] = (r * brightness_) >> 8;
    pixels_[n * 3 + 1] = (g * brightness_) >> 8;
    pixels_[n * 3 + 2] = (b * brightness_) >> 8;
}

void gem_dotstar_set32(size_t n, uint32_t color) {
    gem_dotstar_set(n, color >> 16 & 0xFF, color >> 8 & 0xFF, color & 0xFF);
}

void gem_dotstar_update(const struct GemDotstarCfg* dotstar) {
    gem_spi_write(dotstar->spi, start_frame_, 4);
    for (size_t i = 0; i < dotstar->count; i++) {
        gem_spi_write(dotstar->spi, (uint8_t[]){0xFF}, 1);
        gem_spi_write(dotstar->spi, pixels_ + (i * 3), 3);
    }
    for (size_t i = 0; i < dotstar->count / 2 + 1; i++) { gem_spi_write(dotstar->spi, (uint8_t[]){0x00}, 1); }
}
