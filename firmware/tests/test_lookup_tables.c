/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* Tests for src/gem_voice_params.c */

#include "fix16.h"
#include "gem_config.h"
#include "gem_ramp_table.h"
#include "gem_test.h"

TEST_CASE_BEGIN(lowest)
    uint16_t ramp_cv = gem_ramp_table_lookup(0, F16(0.0));
    munit_assert_uint16(ramp_cv, ==, gem_ramp_table[0].castor_ramp_cv);

    ramp_cv = gem_ramp_table_lookup(1, F16(0.0));
    munit_assert_uint16(ramp_cv, ==, gem_ramp_table[0].pollux_ramp_cv);
TEST_CASE_END

TEST_CASE_BEGIN(lerp_between_2_and_3)
    uint16_t ramp_cv = gem_ramp_table_lookup(0, F16(2.2));

    // This test currently depends on the tables having specific values.
    munit_assert_uint16(ramp_cv, >, gem_ramp_table[4].castor_ramp_cv);
    munit_assert_uint16(ramp_cv, <, gem_ramp_table[5].castor_ramp_cv);
TEST_CASE_END

TEST_CASE_BEGIN(sweep)
    uint16_t last_ramp_cv = 0;

    for (fix16_t i = F16(0); i < F16(7.0); i = fix16_add(i, F16(0.02))) {
        uint16_t ramp_cv = gem_ramp_table_lookup(0, i);
        // printf(
        //     "Sweep: %f yields %u with dac %u\n",
        //     fix16_to_dbl(i),
        //     current_p.period,
        //     current_p.ramp_cv);

        munit_assert_uint16(ramp_cv, >=, last_ramp_cv);
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
