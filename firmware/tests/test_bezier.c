/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* Tests for src/gem_voice_params.c */

#include "fix16.h"
#include "gem_test.h"
#include "wntr_bezier.h"

#define ASSERT_FIX16_CLOSE(val, target, epsilon)                                                                       \
    munit_assert_int32(fix16_abs(fix16_sub(val, target)), <=, F16(epsilon));

#define ASSERT_FIX16_GT(val, target) munit_assert_int32(val, >, F16(target));
#define ASSERT_FIX16_LT(val, target) munit_assert_int32(val, <, F16(target));

TEST_CASE_BEGIN(bezier_1d_mostly_linear)
    fix16_t result = wntr_bezier_cubic_1d(F16(0), F16(0.4), F16(0.6), F16(1.0), F16(0.5));
    ASSERT_FIX16_CLOSE(result, F16(0.5), 0.001);

    for (fix16_t i = F16(0); i < F16(1.0); i = fix16_add(i, F16(0.1))) {
        result = wntr_bezier_cubic_1d(F16(0), F16(0.4), F16(0.6), F16(1.0), i);
        ASSERT_FIX16_CLOSE(result, i, 0.02);
    }
TEST_CASE_END

TEST_CASE_BEGIN(bezier_1d_curvy)
    fix16_t result = wntr_bezier_cubic_1d(F16(0), F16(0.7), F16(0.3), F16(1.0), F16(0.5));
    ASSERT_FIX16_CLOSE(result, F16(0.5), 0.001);

    result = wntr_bezier_cubic_1d(F16(0), F16(0.7), F16(0.3), F16(1.0), F16(0.25));
    ASSERT_FIX16_GT(result, 0.3);

    result = wntr_bezier_cubic_1d(F16(0), F16(0.7), F16(0.3), F16(1.0), F16(0.75));
    ASSERT_FIX16_LT(result, 0.7);
TEST_CASE_END

TEST_CASE_BEGIN(bezier_1d_non_normalized)
    fix16_t result = wntr_bezier_cubic_1d(F16(-1.0), F16(-0.3), F16(0.3), F16(1.0), F16(0.5));
    ASSERT_FIX16_CLOSE(result, F16(0), 0.001);

    result = wntr_bezier_cubic_1d(F16(-1.0), F16(-0.3), F16(0.3), F16(1.0), F16(0.25));
    ASSERT_FIX16_GT(result, -0.5);

    result = wntr_bezier_cubic_1d(F16(-1.0), F16(-0.3), F16(0.3), F16(1.0), F16(0.75));
    ASSERT_FIX16_LT(result, 0.5);
TEST_CASE_END

TEST_CASE_BEGIN(bezier_1d_lut)
    const size_t lut_len = 128;
    fix16_t lut[lut_len];

    wntr_bezier_cubic_1d_generate_lut(F16(-1.0), F16(-0.3), F16(0.3), F16(1.0), lut, lut_len);

    for (fix16_t i = F16(0); i <= F16(1.0); i = fix16_add(i, F16(0.02))) {
        fix16_t calculated = wntr_bezier_cubic_1d(F16(-1.0), F16(-0.3), F16(0.3), F16(1.0), i);
        fix16_t interpolated = wntr_bezier_1d_lut_lookup(lut, lut_len, i);
        ASSERT_FIX16_CLOSE(calculated, interpolated, 0.05);
    }
TEST_CASE_END

static MunitTest test_suite_tests[] = {
    {.name = "1d, mostly linear", .test = test_bezier_1d_mostly_linear},
    {.name = "1d, curvy", .test = test_bezier_1d_curvy},
    {.name = "1d, non-normalized", .test = test_bezier_1d_non_normalized},
    {.name = "1d, look-up table", .test = test_bezier_1d_lut},
    {.test = NULL},
};

MunitSuite test_bezier_suite = {
    .prefix = "bezier: ",
    .tests = test_suite_tests,
    .iterations = 1,
};
