/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* Tests for src/gem_voice_params.c */

#include "fix16.h"
#include "gem_test.h"
#include "gem_voice_param_table.h"
#include "gem_voice_params.h"

TEST_CASE_BEGIN(lowest)
    struct GemVoiceParams vp = {};

    GemVoiceParams_from_cv(F16(0.0), &vp);

    munit_assert_int32(vp.voltage_and_period.voltage, ==, gem_voice_voltage_and_period_table[0].voltage);
    munit_assert_uint32(vp.voltage_and_period.period, ==, gem_voice_voltage_and_period_table[0].period);
    munit_assert_uint16(vp.dac_codes.castor, ==, gem_voice_dac_codes_table[0].castor);
    munit_assert_uint16(vp.dac_codes.pollux, ==, gem_voice_dac_codes_table[0].pollux);
TEST_CASE_END

TEST_CASE_BEGIN(lerp_between_2_and_3)
    struct GemVoiceParams vp = {};

    GemVoiceParams_from_cv(F16(2.2), &vp);

    munit_assert_int32(vp.voltage_and_period.voltage, ==, F16(2.2));

    // This test currently depends on the tables having specific values.

    // Voltage is increasing as table indexes increase
    munit_assert_int32(vp.voltage_and_period.voltage, >, gem_voice_voltage_and_period_table[26].voltage);
    munit_assert_int32(vp.voltage_and_period.voltage, <, gem_voice_voltage_and_period_table[27].voltage);
    // Period is decreasing as table indexes increase
    munit_assert_uint32(vp.voltage_and_period.period, <, gem_voice_voltage_and_period_table[26].period);
    munit_assert_uint32(vp.voltage_and_period.period, >, gem_voice_voltage_and_period_table[27].period);

    munit_assert_uint16(vp.dac_codes.castor, >, gem_voice_dac_codes_table[4].castor);
    munit_assert_uint16(vp.dac_codes.castor, <, gem_voice_dac_codes_table[5].castor);
    munit_assert_uint16(vp.dac_codes.pollux, >, gem_voice_dac_codes_table[4].pollux);
    munit_assert_uint16(vp.dac_codes.pollux, <, gem_voice_dac_codes_table[5].pollux);
TEST_CASE_END

TEST_CASE_BEGIN(sweep)
    struct GemVoiceParams last_p = {.voltage_and_period = {.voltage = F16(0), .period = (2 << 24) - 1}};
    struct GemVoiceParams current_p = {};

    for (fix16_t i = F16(0); i < F16(7.0); i = fix16_add(i, F16(0.02))) {
        GemVoiceParams_from_cv(i, &current_p);
        printf(
            "Sweep: %f yields %u with dac %u & %u\n",
            fix16_to_dbl(i),
            current_p.voltage_and_period.period,
            current_p.dac_codes.castor,
            current_p.dac_codes.pollux);

        munit_assert_int32(current_p.voltage_and_period.voltage, ==, i);
        munit_assert_uint32(current_p.voltage_and_period.period, <, last_p.voltage_and_period.period);
        munit_assert_uint32(current_p.dac_codes.castor, >=, last_p.dac_codes.castor);
        munit_assert_uint32(current_p.dac_codes.pollux, >=, last_p.dac_codes.pollux);

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
    .prefix = "voice params: ",
    .tests = test_suite_tests,
    .iterations = 1,
};
