#include "gem_settings_load_save.h"
#include "gem_config.h"
#include "gem_nvm.h"
#include "printf.h"
#include "sam.h"
#include <stdarg.h>

#define SETTINGS_MARKER 0x65

extern uint8_t _nvm_settings_base_address;

bool GemSettings_check(struct GemSettings* settings) {
    if (settings->adc_gain_corr < 512 || settings->adc_gain_corr > 4096) {
        goto fail;
    }
    if (settings->led_brightness > 255) {
        goto fail;
    }

    return true;

fail:
    printf("Invalid settings data:\n");
    GemSettings_print(settings);
    printf("Resetting to defaults.\n");
    GemSettings_init(settings);
    return false;
}

bool GemSettings_load(struct GemSettings* settings) {
    uint8_t data[GEMSETTINGS_PACKED_SIZE];
    gem_nvm_read((uint32_t)(&_nvm_settings_base_address), data, GEMSETTINGS_PACKED_SIZE + 1);

    if (data[0] != SETTINGS_MARKER) {
        printf("Invalid settings marker.\n");
        goto fail;
    }

    struct StructyResult result = GemSettings_unpack(settings, data + 1);

    if (result.status == STRUCTY_RESULT_OKAY) {
        return GemSettings_check(settings);
    }

    printf("Failed to load settings.\n");

fail:
    printf("Loading default settings.\n");
    GemSettings_init(settings);
    return false;
}

void GemSettings_save(struct GemSettings* settings) {
    uint8_t data[GEMSETTINGS_PACKED_SIZE + 1];
    data[0] = SETTINGS_MARKER;

    GemSettings_check(settings);

    struct StructyResult result = GemSettings_pack(settings, data);

    /* This should basically never happen, so debug trap this. */
    DEBUG_TRAP(result.status != STRUCTY_RESULT_OKAY);

    gem_nvm_write((uint32_t)(&_nvm_settings_base_address), data, GEMSETTINGS_PACKED_SIZE + 1);

    printf("Saved settings: \n");
    GemSettings_print(settings);
}

void GemSettings_erase() {
    /* Just erase the marker byte. */
    uint8_t data[1] = {0xFF};
    gem_nvm_write((uint32_t)(&_nvm_settings_base_address), data, 1);
}
