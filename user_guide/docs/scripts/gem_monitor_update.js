// Hey this file was generated by Structy from
// gem_monitor_update.structy. Maybe don't edit it!

import Struct from "./structy.js";

class GemMonitorUpdate extends Struct {
  static _pack_string = "B?HHHHHHHHBiHIHHHHHHHiBiHIHHHH";
  static _fields = [
    { name: "mode", kind: "uint8", default: 0 },
    { name: "tweaking", kind: "bool", default: false },
    { name: "lfo_knob", kind: "uint16", default: 0 },
    { name: "tweak_lfo_knob", kind: "uint16", default: 0 },
    { name: "castor_pitch_knob", kind: "uint16", default: 0 },
    { name: "castor_pitch_cv", kind: "uint16", default: 0 },
    { name: "castor_pulse_knob", kind: "uint16", default: 0 },
    { name: "castor_pulse_cv", kind: "uint16", default: 0 },
    { name: "castor_tweak_pitch_knob", kind: "uint16", default: 0 },
    { name: "castor_tweak_pulse_knob", kind: "uint16", default: 0 },
    { name: "castor_pitch_behavior", kind: "uint8", default: 0 },
    { name: "castor_pitch", kind: "fix16", default: 0 },
    { name: "castor_pulse_width", kind: "uint16", default: 0 },
    { name: "castor_period", kind: "uint32", default: 0 },
    { name: "castor_ramp", kind: "uint16", default: 0 },
    { name: "pollux_pitch_knob", kind: "uint16", default: 0 },
    { name: "pollux_pitch_cv", kind: "uint16", default: 0 },
    { name: "pollux_pulse_knob", kind: "uint16", default: 0 },
    { name: "pollux_pulse_cv", kind: "uint16", default: 0 },
    { name: "pollux_tweak_pitch_knob", kind: "uint16", default: 0 },
    { name: "pollux_tweak_pulse_knob", kind: "uint16", default: 0 },
    { name: "pollux_reference_pitch", kind: "fix16", default: 0 },
    { name: "pollux_pitch_behavior", kind: "uint8", default: 0 },
    { name: "pollux_pitch", kind: "fix16", default: 0 },
    { name: "pollux_pulse_width", kind: "uint16", default: 0 },
    { name: "pollux_period", kind: "uint32", default: 0 },
    { name: "pollux_ramp", kind: "uint16", default: 0 },
    { name: "loop_time", kind: "uint16", default: 0 },
    { name: "animation_time", kind: "uint16", default: 0 },
    { name: "sample_time", kind: "uint16", default: 0 },
  ];

  static packed_size = 66;

  constructor(values = {}) {
    super(values);
  }
}

export default GemMonitorUpdate;