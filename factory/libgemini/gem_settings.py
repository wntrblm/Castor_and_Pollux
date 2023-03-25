# Hey this file was generated by Structy from
# gem_settings.structy. Maybe don't edit it!

from dataclasses import dataclass
from typing import ClassVar

import structy


@dataclass
class GemSettings(structy.Struct):
    _PACK_STRING : ClassVar[str] = "HhHiiiiiiiiiiH??iiiBBiiHI??"

    PACKED_SIZE : ClassVar[int] = 80
    """The total size of the struct once packed."""

    adc_gain_corr: int = 2048
    """The ADC's internal gain correction register."""

    adc_offset_corr: int = 0
    """The ADC's internal offset correction register."""

    led_brightness: int = 127
    """The front-plate LED brightness."""

    castor_knob_min: structy.Fix16 = -1.2
    """Configuration for the CV knob mins and maxs in volts, defaults to -1.2 to +1.2."""

    castor_knob_max: structy.Fix16 = 1.2

    pollux_knob_min: structy.Fix16 = -1.2

    pollux_knob_max: structy.Fix16 = 1.2

    chorus_max_intensity: structy.Fix16 = 0.05
    """Maximum amount that the chorus can impact Pollux's frequency."""

    lfo_1_frequency: structy.Fix16 = 0.2
    """The default LFO frequency in hertz."""

    cv_offset_error: structy.Fix16 = 0.0
    """Error correction for the ADC readings for the CV input."""

    cv_gain_error: structy.Fix16 = 1.0

    removed_smooth_initial_gain: structy.Fix16 = 0.1
    """(Removed) Pitch input CV smoothing parameters."""

    removed_smooth_sensitivity: structy.Fix16 = 30.0

    zero_detection_threshold: int = 350
    """ This is used to detect whether the pitch CV inputs have something
            patched. Basically, C&P checks if the input is near 0V and if it is,
            it assumes nothing is patched. If you routinely send 0V CV into C&P
            this can cause unexpected behavior, so in the case you can disable
            zero detection with zero_detection_enabled.

            The default is 350 which corresponds to just over 0V for C&P II.

            (previously named pollux_follower_threshold)
        """

    removed_castor_lfo_pwm: bool = False
    """(Removed) Route LFO to PWM for oscillators"""

    removed_pollux_lfo_pwm: bool = False

    pitch_knob_nonlinearity: structy.Fix16 = 0.6
    """The firmware adjusts the pitch CV knobs so that it's easier to tune
            the oscillators. It does this by spreading the values near the center
            of the knob out so that the range at the center is more fine. This
            makes the knobs non-linear. This setting controls how "intense"
            this non-linearity is. A value of 0.33 is equivalent to a linear
            response. Higher values make it easier and easier to tune the
            oscillator but sacrifice the range, values lower than 0.33 will
            make it harder to tune and aren't recommended."""

    base_cv_offset: structy.Fix16 = 1.0
    """The base CV offset applied to the pitch inputs."""

    lfo_2_frequency_ratio: structy.Fix16 = 2
    """The ratio of the second LFO's frequency to the first."""

    lfo_1_waveshape: int = 0
    """LFO 1's waveshape."""

    lfo_2_waveshape: int = 0
    """LFO 2's waveshape."""

    lfo_1_factor: structy.Fix16 = 1
    """LFO 1's factor."""

    lfo_2_factor: structy.Fix16 = 0
    """LFO 2's factor."""

    pulse_width_bitmask: int = 4095
    """
        The bitmask applied to the pulse width DAC output. This allows emulating
        the behavior of Castor & Pollux's original firmware where timing issues
        caused pulse width modulation to "step".
        """

    osc8m_freq: int = 8000000
    """
        Measured 8MHz oscillator frequency, used to fine tune the output pitch.
        """

    zero_detection_enabled: bool = True
    """
        Enables or disables zero volt detection used to check whether a patch
        cable is present at the pitch inputs.
        """

    quantization_enabled: bool = True
    """
        Enables or disables quantization when Castor is in "Coarse" mode
        """
