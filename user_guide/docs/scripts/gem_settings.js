// Hey this file was generated by Structy from
// gem_settings.structy. Maybe don't edit it!

import Struct from "./structy.js";

class GemSettings extends Struct {
  static _pack_string = "HhHiiiiiiiiiiH??iiiBBiiHI??";
  static _fields = [
    { name: "adc_gain_corr", kind: "uint16", default: 2048 },
    { name: "adc_offset_corr", kind: "int16", default: 0 },
    { name: "led_brightness", kind: "uint16", default: 127 },
    { name: "castor_knob_min", kind: "fix16", default: -1.2 },
    { name: "castor_knob_max", kind: "fix16", default: 1.2 },
    { name: "pollux_knob_min", kind: "fix16", default: -1.2 },
    { name: "pollux_knob_max", kind: "fix16", default: 1.2 },
    { name: "chorus_max_intensity", kind: "fix16", default: 0.05 },
    { name: "lfo_1_frequency", kind: "fix16", default: 0.2 },
    { name: "cv_offset_error", kind: "fix16", default: 0.0 },
    { name: "cv_gain_error", kind: "fix16", default: 1.0 },
    { name: "removed_smooth_initial_gain", kind: "fix16", default: 0.1 },
    { name: "removed_smooth_sensitivity", kind: "fix16", default: 30.0 },
    { name: "zero_detection_threshold", kind: "uint16", default: 350 },
    { name: "removed_castor_lfo_pwm", kind: "bool", default: false },
    { name: "removed_pollux_lfo_pwm", kind: "bool", default: false },
    { name: "pitch_knob_nonlinearity", kind: "fix16", default: 0.6 },
    { name: "base_cv_offset", kind: "fix16", default: 1.0 },
    { name: "lfo_2_frequency_ratio", kind: "fix16", default: 2 },
    { name: "lfo_1_waveshape", kind: "uint8", default: 0 },
    { name: "lfo_2_waveshape", kind: "uint8", default: 0 },
    { name: "lfo_1_factor", kind: "fix16", default: 1 },
    { name: "lfo_2_factor", kind: "fix16", default: 0 },
    { name: "pulse_width_bitmask", kind: "uint16", default: 4095 },
    { name: "osc8m_freq", kind: "uint32", default: 8000000 },
    { name: "zero_detection_enabled", kind: "bool", default: true },
    { name: "quantization_enabled", kind: "bool", default: true },
  ];

  static packed_size = 80;

  constructor(values = {}) {
    super(values);
  }
}

export default GemSettings;
