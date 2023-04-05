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
#include "wntr_uint12.h"

const struct WntrErrorCorrection error_correction = {.offset = F16(0), .gain = F16(1)};
struct GemOscillator osc = {
    .number = 0,
    .pitch_offset = F16(1.0),
    .pitch_cv_min = F16(0),
    .pitch_cv_max = F16(6.0),
    .lfo_pitch_factor = F16(1.0),
    .pitch_knob_min = F16(-1.0),
    .pitch_knob_max = F16(1.0),
    .pulse_width_bitmask = 0xFFFF,
    .can_follow = false,
    .zero_detection_enabled = true,
    .zero_detection_threshold = 10,
    .quantization_enabled = true,
};

TEST_CASE_BEGIN(coarse_pitch)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_NORMAL,
        .tweak_pitch_knob_code = UINT16_MAX,
    };

    // Scenario:
    // - Nothing connected to pitch CV in.
    // - Pitch knob fully CCW.
    //
    // This should result in the lowest note, so it should give us 1.0V as
    // the final pitch, since osc->pitch_offset is 1.0.

    inputs.pitch_cv_code = 4095;
    inputs.pitch_knob_code = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(1.0), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is dead center.
    //
    // Thi should be in the middle of the range (0 -> 6, so 3) plus the pitch
    // offset, so 4.0.

    inputs.pitch_cv_code = 4095;
    inputs.pitch_knob_code = 2048;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.0), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is barely to the right of dead
    //   center
    //
    // This should quantize to the closest semitone, so it should be the same
    // as the previous scenario.

    inputs.pitch_cv_code = 4095;
    inputs.pitch_knob_code = 2052;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.0), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is fully CW
    //
    // This should also be "coarse" mode but the note should be in the top
    // of the range (0 -> 6, so 6) plus the pitch offset, so 7.0.

    inputs.pitch_cv_code = 4095;
    inputs.pitch_knob_code = 4095;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(7.0), 0.01);
TEST_CASE_END

TEST_CASE_BEGIN(fine_pitch)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_NORMAL,
        .tweak_pitch_knob_code = UINT16_MAX,
    };

    // Scenario:
    // - Pitch CV at middle of the range (3.0V)
    // - Pitch knob dead center
    //
    // This should result in "fine" mode and the same value that's passed
    // in as the pitch CV plus the base offset.
    inputs.pitch_cv_code = 2048;
    inputs.pitch_knob_code = 2048;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.00), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is fully CCW.
    //
    // This should result in a one octave lower.
    inputs.pitch_cv_code = 2048;
    inputs.pitch_knob_code = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(3.00), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is fully CW.
    //
    // This should result in a one octave higher.
    inputs.pitch_cv_code = 2048;
    inputs.pitch_knob_code = 4095;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(5.00), 0.01);
TEST_CASE_END

TEST_CASE_BEGIN(extra_fine_pitch)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_NORMAL,
        .pitch_cv_code = 2048,
        .pitch_knob_code = 2048,
    };

    // Scenario:
    // - Pitch CV at middle of the range (3.0V)
    // - Pitch knob dead center
    // - Pitch tweak knob dead center
    //
    // This should result in the pitch CV passing through unmodified except
    // for the base offset (1.0V)
    inputs.tweak_pitch_knob_code = 2048;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.00), 0.01);

    // Scenario:
    // - Same as above except the pitch tweak knob is fully CCW.
    //
    // This should result in the same as above -0.2.
    inputs.tweak_pitch_knob_code = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(3.80), 0.01);

    // Scenario:
    // - Same as above except the pitch tweak knob is fully CW.
    //
    // This should result in the same as above +0.2.
    inputs.tweak_pitch_knob_code = 4095;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.20), 0.01);
TEST_CASE_END

TEST_CASE_BEGIN(follow_pitch)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    osc.number = 1;  // Pollux

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_NORMAL,
        .pitch_cv_code = 4095,
        .tweak_pitch_knob_code = UINT16_MAX,
    };

    // Scenario:
    // - Nothing connected to pitch CV in.
    // - Pitch knob dead center
    //
    // This should result in "follow" mode and the same value that's passed
    // in as the reference pitch.
    inputs.reference_pitch = F16(3.33);
    inputs.pitch_knob_code = 2048;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(3.33), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is fully CCW.
    //
    // This should result in a one octave lower than the reference pitch.
    inputs.reference_pitch = F16(3.33);
    inputs.pitch_knob_code = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(2.33), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is fully CW.
    //
    // This should result in a one octave higher than the reference pitch.
    inputs.reference_pitch = F16(3.33);
    inputs.pitch_knob_code = 4095;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.33), 0.01);
TEST_CASE_END

TEST_CASE_BEGIN(normal_mode_lfo_fm)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_NORMAL,
        .pitch_knob_code = 2048,
        .tweak_pitch_knob_code = UINT16_MAX,
    };

    // Scenario:
    // - Pollux with 3V patched in
    // - LFO at max amplitude
    // - LFO knob all the way up
    // - LFO pitch factor is 0.5
    //
    // This should modulate the pitch up 0.5V
    osc.number = 1;
    inputs.pitch_cv_code = 2048;
    inputs.lfo_amplitude = F16(1.0);
    inputs.lfo_knob_code = 4095;
    osc.lfo_pitch_factor = F16(0.5);

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.5), 0.01);

    // Scenario:
    // - Same as above, but with Castor
    //
    // This shouldn't modulate Castor's pitch
    osc.number = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4), 0.01);

    // Scenario:
    // - Pulse width knobs at dead center
    // - Pulse width input at 400 code points (about 500mV)
    //
    // Pulse width should be the sum of those two.
    //
    inputs.pulse_knob_code = 2048;
    inputs.pulse_cv_code = 400;

    GemOscillator_update(&osc, inputs);
    munit_assert_int16(osc.pulse_width, ==, 2047 + 400);
TEST_CASE_END

TEST_CASE_BEGIN(pwm_mode)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_LFO_PWM,
        .pitch_knob_code = 2048,
        .tweak_pitch_knob_code = UINT16_MAX,
        .pulse_knob_code = 2048,
        .tweak_pulse_knob_code = UINT16_MAX,
    };

    // Scenario:
    // - Pollux
    // - Pitch knob dead center
    // - Pitch input at 3V
    // - LFO amplitude at -1.0
    //
    // In PWM mode, Pollux's pitch should *not* be modulated by the LFO.
    osc.number = 1;
    inputs.pitch_knob_code = 2048;
    inputs.pitch_cv_code = 2048;
    inputs.lfo_amplitude = F16(-1.0);
    inputs.lfo_knob_code = 4095;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4), 0.01);

    // Scenario:
    // - Pulse knob at dead center
    // - Pulse CV unpatched (0)
    // - LFO amplitude at -1.0
    //
    // This should modulate the pulse width by 50%
    inputs.pulse_knob_code = 2048;
    inputs.pulse_cv_code = 0;
    inputs.lfo_amplitude = F16(-1.0);

    GemOscillator_update(&osc, inputs);
    munit_assert_int16(osc.pulse_width, ==, 2048 - 1025);

    // Scenario:
    // - Same as above but the LFO's amplitude is at +1.0
    inputs.lfo_amplitude = F16(1.0);

    GemOscillator_update(&osc, inputs);
    munit_assert_int16(osc.pulse_width, ==, 2048 + 1023);

    // Scenario:
    // - Pulse knob fully CCW
    // - Pulse CV input at 2.5
    //
    // This should also modulate the pulse width by 50%
    inputs.pulse_knob_code = 0;
    inputs.pulse_cv_code = 2048;
    inputs.lfo_amplitude = F16(-1.0);

    GemOscillator_update(&osc, inputs);
    munit_assert_int16(osc.pulse_width, ==, 2048 - 1025);

    // Scenario:
    // - Pulse knob fully CCW
    // - Pulse CV input at 0
    // - Pulse tweak knob slightly left of center.
    //
    // The pulse width shouldn't be modulated, but should be equal to the
    // tweak knob.
    inputs.pulse_knob_code = 0;
    inputs.pulse_cv_code = 0;
    inputs.tweak_pulse_knob_code = 2000;
    inputs.lfo_amplitude = F16(-1.0);

    GemOscillator_update(&osc, inputs);
    munit_assert_int16(osc.pulse_width, ==, 1999);
TEST_CASE_END

TEST_CASE_BEGIN(fm_mode)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    osc.lfo_pitch_factor = F16(1.0);

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_LFO_FM,
        .tweak_pitch_knob_code = UINT16_MAX,
    };

    // Scenario:
    // - Pitch knob dead center
    // - Pulse knob dead center
    // - Pitch input at 3V
    // - Pulse input at 0V
    // - LFO amplitude at -1.0
    //
    // In FM mode, the pitch should be modulated based on the pulse knob, so
    // in this case it should be modulated by 50%, so (3V + 1V) + (-1 * 0.5);
    inputs.pitch_knob_code = 2048;
    inputs.pitch_cv_code = 2048;
    inputs.pulse_knob_code = 2048;
    inputs.pulse_cv_code = 0;
    inputs.lfo_amplitude = F16(-1.0);

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(3.5), 0.01);

    // Scenario
    // - Same as above, but with the pulse knob all the way CCW.
    //
    // Should not offset the pitch at all.
    inputs.pulse_knob_code = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.0), 0.01);

    // Scenario:
    // - Same as above, just with the LFO amplitude at 1.0.
    inputs.pulse_knob_code = 2048;
    inputs.lfo_amplitude = F16(1.0);

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.5), 0.01);

    // Scenario:
    // - Same as above, but with the pitch tweak knob fully CCW.
    //
    // Should offset the pitch by the tweak amount.
    inputs.tweak_pitch_knob_code = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.3), 0.01);

    // Scenario:
    // - Pulse tweak knob fully CW
    //
    // Pulse width should be determined solely by the pulse tweak knob
    osc.pulse_width_bitmask = 0xFFFF;
    inputs.tweak_pulse_knob_code = 2060;

    GemOscillator_update(&osc, inputs);
    munit_assert_int16(osc.pulse_width, ==, 2059);
TEST_CASE_END

TEST_CASE_BEGIN(hard_sync_mode)
    gem_oscillator_init(error_correction, F16(0.6));
    GemOscillator_init(&osc);

    osc.number = 1;  // Pollux

    struct GemOscillatorInputs inputs = {
        .mode = GEM_MODE_HARD_SYNC,
        .pitch_cv_code = 4095,
        .tweak_pitch_knob_code = UINT16_MAX,
    };

    // Scenario:
    // - Nothing connected to pitch CV in.
    // - Pitch knob dead center
    //
    // This should result in Pollux's frequency being 1.5V higher than
    // Castor's
    inputs.reference_pitch = F16(3.33);
    inputs.pitch_knob_code = 2048;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(4.83), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is fully CCW.
    //
    // This should result in Pollux's frequency being the exact same as
    // Castor's
    inputs.pitch_knob_code = 0;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(3.33), 0.01);

    // Scenario:
    // - Same as above except the pitch knob is fully CW.
    //
    // This should result in Pollux's frequency being 3V higher than
    // Castor's
    inputs.pitch_knob_code = 4095;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(6.33), 0.01);

    // Scenario:
    // - Pollux now has a 3V pitch input
    // - Pitch knob is dead center
    //
    // This should result in Pollux's frequency being 1.5V higher than
    // the input voltage *plus* the 1V base offset.
    inputs.pitch_cv_code = 2048;
    inputs.pitch_knob_code = 2048;

    GemOscillator_update(&osc, inputs);
    ASSERT_FIX16_CLOSE(osc.pitch, F16(5.5), 0.01);
TEST_CASE_END

static MunitTest test_suite_tests[] = {
    {.name = "coarse pitch", .test = test_coarse_pitch},
    {.name = "follow pitch", .test = test_follow_pitch},
    {.name = "fine pitch", .test = test_fine_pitch},
    {.name = "extra fine pitch", .test = test_extra_fine_pitch},
    {.name = "normal mode lfo fm", .test = test_normal_mode_lfo_fm},
    {.name = "pwm mode", .test = test_pwm_mode},
    {.name = "fm mode", .test = test_fm_mode},
    {.name = "hard sync", .test = test_hard_sync_mode},
    {.test = NULL},
};

MunitSuite test_oscillator_suite = {
    .prefix = "oscillator: ",
    .tests = test_suite_tests,
    .iterations = 1,
};
