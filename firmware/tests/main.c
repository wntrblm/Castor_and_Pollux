/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_test.h"

MunitSuite meta_suite = {.prefix = "gemini: ", .iterations = 1};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
    MunitSuite suites[] = {
        test_midi_core_suite, test_voice_params_suite, test_bezier_suite, test_oscillator_suite, {.prefix = NULL}};
    meta_suite.suites = suites;
    return munit_suite_main(&meta_suite, (void*)"gemini", argc, argv);
}
