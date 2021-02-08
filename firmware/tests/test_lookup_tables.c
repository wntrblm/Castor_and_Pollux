/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* Tests for src/gem_voice_params.c */

#include "fix16.h"
#include "gem_test.h"
#include "gem_config.h"
#include "gem_lookup_tables.h"
#include "gem_oscillator.h"

TEST_CASE_BEGIN(lowest)
    struct GemOscillatorOutputs vp = {};

    GemOscillatorOutputs_calculate(0, F16(0.0), &vp);

    munit_assert_int32(vp.pitch_cv, ==, gem_pitch_table[0].pitch_cv);
    munit_assert_uint32(vp.period, ==, gem_pitch_table[0].period);
    munit_assert_uint16(vp.ramp_cv, ==, gem_ramp_table[0].castor_ramp_cv);

    GemOscillatorOutputs_calculate(1, F16(0.0), &vp);

    munit_assert_int32(vp.pitch_cv, ==, gem_pitch_table[0].pitch_cv);
    munit_assert_uint32(vp.period, ==, gem_pitch_table[0].period);
    munit_assert_uint16(vp.ramp_cv, ==, gem_ramp_table[0].pollux_ramp_cv);
TEST_CASE_END

TEST_CASE_BEGIN(lerp_between_2_and_3)
    struct GemOscillatorOutputs vp = {};

    GemOscillatorOutputs_calculate(0, F16(2.2), &vp);

    munit_assert_int32(vp.pitch_cv, ==, F16(2.2));

    // This test currently depends on the tables having specific values.

    // Voltage is increasing as table indexes increase
    munit_assert_int32(vp.pitch_cv, >, gem_pitch_table[26].pitch_cv);
    munit_assert_int32(vp.pitch_cv, <, gem_pitch_table[27].pitch_cv);
    // Period is decreasing as table indexes increase
    munit_assert_uint32(vp.period, <, gem_pitch_table[26].period);
    munit_assert_uint32(vp.period, >, gem_pitch_table[27].period);

    munit_assert_uint16(vp.ramp_cv, >, gem_ramp_table[4].castor_ramp_cv);
    munit_assert_uint16(vp.ramp_cv, <, gem_ramp_table[5].castor_ramp_cv);
TEST_CASE_END

TEST_CASE_BEGIN(sweep)
    struct GemOscillatorOutputs last_p = {.pitch_cv = F16(0), .period = (2 << 24) - 1};
    struct GemOscillatorOutputs current_p = {};

    for (fix16_t i = F16(0); i < F16(7.0); i = fix16_add(i, F16(0.02))) {
        GemOscillatorOutputs_calculate(0, i, &current_p);
        printf(
            "Sweep: %f yields %u with dac %u\n",
            fix16_to_dbl(i),
            current_p.period,
            current_p.ramp_cv);

        munit_assert_int32(current_p.pitch_cv, ==, i);
        munit_assert_uint32(current_p.period, <, last_p.period);
        munit_assert_uint32(current_p.ramp_cv, >=, last_p.ramp_cv);

        last_p = current_p;
    }
TEST_CASE_END

static MunitTest test_suite_tests[] = {
    {.name = "lowest", .test = test_lowest},
    {.name = "lerp between 2 -> 3 volts", .test = test_lerp_between_2_and_3},
    {.name = "sweep across range", .test = test_sweep},
    {.test = NULL},
};

MunitSuite test_voice_params_suite = {
    .prefix = "voice outputs: ",
    .tests = test_suite_tests,
    .iterations = 1,
};
