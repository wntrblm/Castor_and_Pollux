/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_settings_load_save.h"
#include "gem_config.h"
#include "printf.h"
#include "sam.h"
#include "wntr_assert.h"
#include "wntr_nvm.h"
#include <stdarg.h>

#define SETTINGS_MARKER_V1 0x65
#define SETTINGS_MARKER_V2 0x66
#define SETTINGS_MARKER_V3 0x67
#define SETTINGS_MARKER_V4 0x68
#define SETTINGS_MARKER_V5 0x69
#define SETTINGS_MARKER_MIN SETTINGS_MARKER_V1
#define SETTINGS_MARKER_MAX SETTINGS_MARKER_V5

#define LIMIT_F16_FIELD(field, min, max)                                                                               \
    if (settings->field < F16(min) || settings->field > F16(max)) {                                                    \
        settings->field = defaults.field;                                                                              \
    }

#define LIMIT_INT_FIELD(field, min, max)                                                                               \
    if (settings->field < min || settings->field > max) {                                                              \
        settings->field = defaults.field;                                                                              \
    }

#define DEFAULT_FIELD(field) settings->field = defaults.field;

extern uint8_t _nvm_settings_base_address;

bool GemSettings_check(uint8_t marker, struct GemSettings* settings) {
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

    LIMIT_F16_FIELD(castor_knob_max, 0.0, 10.0);
    LIMIT_F16_FIELD(castor_knob_min, -10.0, 0.0);
    LIMIT_F16_FIELD(pollux_knob_max, 0.0, 10.0);
    LIMIT_F16_FIELD(pollux_knob_min, -10.0, 0.0);
    LIMIT_F16_FIELD(chorus_max_intensity, 0.0, 1.0);
    LIMIT_F16_FIELD(lfo_1_frequency, 0.0, 50.0);
    LIMIT_F16_FIELD(pitch_knob_nonlinearity, 0.3, 1.0);

    /* V2 added base_cv_offset field. */
    if (marker < SETTINGS_MARKER_V2) {
        printf("Upgrading settings from v1 to v2.\n");
        DEFAULT_FIELD(base_cv_offset);
    }
    LIMIT_F16_FIELD(base_cv_offset, 0.0, 5.0);

    /* V3 added several lfo options. */
    if (marker < SETTINGS_MARKER_V3) {
        printf("Upgrading settings from v2 to v3.\n");
        DEFAULT_FIELD(lfo_2_frequency_ratio)
        DEFAULT_FIELD(lfo_1_waveshape)
        DEFAULT_FIELD(lfo_2_waveshape)
        DEFAULT_FIELD(lfo_1_factor)
        DEFAULT_FIELD(lfo_2_factor)
    }
    LIMIT_F16_FIELD(lfo_2_frequency_ratio, 0.0, 10.0);
    LIMIT_INT_FIELD(lfo_1_waveshape, 0, 4);
    LIMIT_INT_FIELD(lfo_2_waveshape, 0, 4);
    LIMIT_F16_FIELD(lfo_1_factor, 0.0, 1.0);
    LIMIT_F16_FIELD(lfo_2_factor, 0.0, 1.0);

    /* V4 added pulse width bitmask */
    if (marker < SETTINGS_MARKER_V4) {
        printf("Upgrading settings from v3 to v4.\n");
        DEFAULT_FIELD(pulse_width_bitmask);
    }

    /* V5 added osc8m_freq, zero_detection_enabled, and quantization_enabled */
    if (marker < SETTINGS_MARKER_V5) {
        printf("Upgrading settings from v4 to v5.\n");
        DEFAULT_FIELD(osc8m_freq);
        DEFAULT_FIELD(zero_detection_enabled);
        DEFAULT_FIELD(quantization_enabled);
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
    uint8_t data[GEMSETTINGS_PACKED_SIZE + 1];

    // NOLINTNEXTLINE(clang-diagnostic-pointer-to-int-cast)
    wntr_nvm_read((uint32_t)(&_nvm_settings_base_address), data, GEMSETTINGS_PACKED_SIZE + 1);

    uint8_t marker = data[0];

    if (marker < SETTINGS_MARKER_MIN || marker > SETTINGS_MARKER_MAX) {
        printf("Invalid settings marker.\n");
        goto fail;
    }

    struct StructyResult result = GemSettings_unpack(settings, data + 1);

    if (result.status == STRUCTY_RESULT_OKAY) {
        return GemSettings_check(marker, settings);
    }

    printf("Failed to load settings.\n");

fail:
    printf("Loading default settings.\n");
    GemSettings_init(settings);
    return false;
}

void GemSettings_save(struct GemSettings* settings) {
    uint8_t data[GEMSETTINGS_PACKED_SIZE + 1];
    data[0] = SETTINGS_MARKER_V5;

    GemSettings_check(data[0], settings);

    struct StructyResult result = GemSettings_pack(settings, data + 1);

    /* This should basically never happen with structy, but assert it anyway. */
    WNTR_ASSERT(result.status == STRUCTY_RESULT_OKAY);

    // NOLINTNEXTLINE(clang-diagnostic-pointer-to-int-cast)
    wntr_nvm_write((uint32_t)(&_nvm_settings_base_address), data, GEMSETTINGS_PACKED_SIZE + 1);

    printf("Saved settings: \n");
    GemSettings_print(settings);
}

void GemSettings_erase() {
    /* Just erase the marker byte. */
    uint8_t data[1] = {0xFF};
    // NOLINTNEXTLINE(clang-diagnostic-pointer-to-int-cast)
    wntr_nvm_write((uint32_t)(&_nvm_settings_base_address), data, 1);
}
