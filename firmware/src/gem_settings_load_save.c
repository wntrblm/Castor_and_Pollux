#include "gem_settings_load_save.h"
#include "gem_config.h"
#include "gem_nvm.h"
#include "printf.h"
#include "sam.h"
#include <stdarg.h>

#define SETTINGS_MARKER 0x65

extern uint8_t _nvm_settings_base_address;

bool GemSettings_check(struct GemSettings* settings) {
    /* This can't be fixed. If the ADC stuff is out of whack we gotta fail. */
    if (settings->adc_gain_corr < 512 || settings->adc_gain_corr > 4096) {
        goto fail;
    }

    struct GemSettings defaults;
    GemSettings_init(&defaults);

    /* These settings can be repaired/fixed. */
    if (settings->led_brightness > 255) {
        settings->led_brightness = defaults.led_brightness;
    }

#define LIMIT_F16_FIELD(field, min, max)                                                                               \
    if (settings->field < F16(min) || settings->field > F16(max)) {                                                    \
        settings->field = defaults.field;                                                                              \
    }

    LIMIT_F16_FIELD(castor_knob_max, 0.0, 10.0);
    LIMIT_F16_FIELD(castor_knob_min, -10.0, 0.0);
    LIMIT_F16_FIELD(pollux_knob_max, 0.0, 10.0);
    LIMIT_F16_FIELD(pollux_knob_min, -10.0, 0.0);
    LIMIT_F16_FIELD(chorus_max_intensity, 0.0, 1.0);
    LIMIT_F16_FIELD(lfo_frequency, 0.0, 50.0);
    LIMIT_F16_FIELD(smooth_initial_gain, 0.0, 1.0);
    LIMIT_F16_FIELD(smooth_sensitivity, 0.0, 100.0);
    LIMIT_F16_FIELD(pitch_knob_nonlinearity, 0.3, 1.0);

    return true;

fail:
    printf("Invalid settings data:\n");
    GemSettings_print(settings);
    printf("Resetting to defaults.\n");
    GemSettings_init(settings);
    return false;
}

bool GemSettings_load(struct GemSettings* settings) {
    uint8_t data[GEMSETTINGS_PACKED_SIZE + 1];
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

    struct StructyResult result = GemSettings_pack(settings, data + 1);

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
