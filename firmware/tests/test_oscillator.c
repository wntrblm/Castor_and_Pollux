/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* Tests for src/gem_voice_params.c */

#include "fix16.h"
#include "gem_config.h"
#include "gem_oscillator.h"
#include "gem_test.h"

const struct WntrErrorCorrection error_correction = {.offset = F16(0), .gain = F16(1)};

TEST_CASE_BEGIN(update)
    gem_oscillator_init(error_correction, F16(0.6));

    struct GemOscillator osc = {};
    GemOscillator_init(&osc, 0, 0, 1, 2, 3, F16(1.0), F16(1.0), F16(1.0), F16(-1.5), F16(1.5), false, false);

    struct GemOscillatorInputs inputs = {.adc = (uint32_t[]){4095, 2048, 4095, 2048}};
    GemOscillator_update(&osc, inputs);

    /*
        Pitch CV is calculated from adc[0] which is 4095- the bottom of the
        range, therefore, this should be 1.0 (GEM_CV_BASE_OFFSET).
    */
    ASSERT_FIX16_CLOSE(osc.pitch_cv, F16(1.0), 0.01);

    /*
        Pitch knob is calculated from adc[1] which is 2048 - the middle of
        the range, therefore, this should be 0.0 since the range is -1.5 to 1.5
    */
    ASSERT_FIX16_CLOSE(osc.pitch_knob, F16(0.0), 0.01);

    /*
        Pitch should just be the sum of the two above, so 1.0.
    */
    ASSERT_FIX16_CLOSE(osc.pitch, F16(1.0), 0.01);

    /*
        Pulse width CV is calculated from adc[2] which is 4095. This gets
        inverted, so it should be 0.
    */
    munit_assert_uint16(osc.pulse_width_cv, ==, 0);

    /*
        Pitch knob is calculated from adc[3] which is 2048. This gets
        inverted, so it'll be 2047.
    */
    munit_assert_uint16(osc.pulse_width_knob, ==, 2047);

    /*
        Pulse width is just the sum of the above, so 2047.
    */
    munit_assert_uint16(osc.pulse_width, ==, 2047);

    /*
        Test setting the pitch CV halfway- should be right in the middle
        of the range (0-6) so 3.0 plus the offset 1.0, so 4.0.
    */
    inputs.adc = (uint32_t[]){2047, 2048, 4095, 2048};
    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch_cv, F16(4.0), 0.01);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.0), 0.01);

    /*
        Test moving the knob fully CCW and CW- should offset the pitch CV
        by -1.5 and 1.5.
    */
    inputs.adc = (uint32_t[]){2047, 4095, 4095, 2048};
    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch_knob, F16(-1.5), 0.01);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.0 - 1.5), 0.01);

    inputs.adc = (uint32_t[]){2047, 0, 4095, 2048};
    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch_knob, F16(1.5), 0.01);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.0 + 1.5), 0.01);

    /*
        Test setting the pulse width CV halfway.
    */
    inputs.adc = (uint32_t[]){4095, 4095, 2048, 4095};
    GemOscillator_update(&osc, inputs);
    munit_assert_uint16(osc.pulse_width_cv, ==, 2047);
    munit_assert_uint16(osc.pulse_width, ==, 2047);

    /*
        Testing moving the pulse width knob CW and CCW.
    */
    inputs.adc = (uint32_t[]){4095, 4095, 3072, 1023};
    GemOscillator_update(&osc, inputs);
    munit_assert_uint16(osc.pulse_width_knob, ==, 3072);
    munit_assert_uint16(osc.pulse_width, ==, 4095);

    inputs.adc = (uint32_t[]){4095, 4095, 1023, 3071};
    GemOscillator_update(&osc, inputs);
    munit_assert_uint16(osc.pulse_width_knob, ==, 1024);
    munit_assert_uint16(osc.pulse_width, ==, 4095);

TEST_CASE_END

static MunitTest test_suite_tests[] = {
    {.name = "update", .test = test_update},
    {.test = NULL},
};

MunitSuite test_oscillator_suite = {
    .prefix = "oscillator: ",
    .tests = test_suite_tests,
    .iterations = 1,
};
