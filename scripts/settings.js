import GemSettings from "./gem_settings.js";
import { teeth_decode, teeth_encode } from "./teeth.js";

const midi_port_name = "Gemini";
const settings_form = document.getElementById("settings_editor");
const save_button = document.getElementById("save_button");
const dangerous_fields = [
  ...settings_form.querySelectorAll("input[readonly]"),
  ...settings_form.querySelectorAll("input[disabled]"),
];
const allow_danger_input = document.getElementById("allow_danger");
const connect_button = document.getElementById("connect");
const connect_info = document.getElementById("connect_info");
let gemini_firmware_version = null;

function settings_from_form() {
  const form_data = new FormData(settings_form);

  const settings = new GemSettings({
    adc_gain_corr: parseInt(form_data.get("adc_gain_corr")),
    adc_offset_corr: parseInt(form_data.get("adc_offset_corr")),
    led_brightness: parseInt(form_data.get("led_brightness")),
    castor_knob_min: parseFloat(form_data.get("castor_knob_min")),
    castor_knob_max: parseFloat(form_data.get("castor_knob_max")),
    pollux_knob_min: parseFloat(form_data.get("pollux_knob_min")),
    pollux_knob_max: parseFloat(form_data.get("pollux_knob_max")),
    chorus_max_intensity: parseFloat(form_data.get("chorus_max_intensity")),
    lfo_frequency: parseFloat(form_data.get("lfo_frequency")),
    cv_offset_error: parseFloat(form_data.get("cv_offset_error")),
    cv_gain_error: parseFloat(form_data.get("cv_gain_error")),
    smooth_initial_gain: parseFloat(form_data.get("smooth_initial_gain")),
    smooth_sensitivity: parseFloat(form_data.get("smooth_sensitivity")),
    castor_lfo_pwm: form_data.get("castor_lfo_pwm") === "on" ? true : false,
    pollux_lfo_pwm: form_data.get("pollux_lfo_pwm") === "on" ? true : false,
    pitch_knob_nonlinearity: parseFloat(
      form_data.get("pitch_knob_nonlinearity")
    ),
  });

  return settings;
}

function form_from_settings(settings) {
  settings_form.adc_gain_corr.value = settings.adc_gain_corr;
  settings_form.adc_offset_corr.value = settings.adc_offset_corr;
  settings_form.led_brightness.value = settings.led_brightness;
  settings_form.castor_knob_min.value = settings.castor_knob_min.toFixed(2);
  settings_form.castor_knob_max.value = settings.castor_knob_max.toFixed(2);
  settings_form.pollux_knob_min.value = settings.pollux_knob_min.toFixed(2);
  settings_form.pollux_knob_max.value = settings.pollux_knob_max.toFixed(2);
  settings_form.chorus_max_intensity.value = settings.chorus_max_intensity.toFixed(
    2
  );
  settings_form.lfo_frequency.value = settings.lfo_frequency.toFixed(2);
  settings_form.cv_offset_error.value = settings.cv_offset_error.toFixed(2);
  settings_form.cv_gain_error.value = settings.cv_gain_error.toFixed(2);
  settings_form.smooth_initial_gain.value = settings.smooth_initial_gain.toFixed(
    2
  );
  settings_form.smooth_sensitivity.value = settings.smooth_sensitivity.toFixed(
    2
  );
  settings_form.pollux_follower_threshold.value =
    settings.pollux_follower_threshold;
  settings_form.castor_lfo_pwm.checked = settings.castor_lfo_pwm;
  settings_form.pollux_lfo_pwm.checked = settings.pollux_lfo_pwm;
  settings_form.pitch_knob_nonlinearity.value =
    settings.pitch_knob_nonlinearity;

  /* Trigger change event for inputs on the form. */
  for (const elem of settings_form.getElementsByTagName("input")) {
    elem.dispatchEvent(new Event("input"));
  }
}

/*
    WebMIDI interface.
*/

let midi_input = null;
let midi_output = null;

async function midi_send_and_receive(data) {
  const done = new Promise(function (resolve, reject) {
    midi_input.onmidimessage = function (msg) {
      resolve(msg);
    };
    midi_output.send(data);
  });

  return await done;
}

async function load_settings_from_device() {
  /* First, get the firmware version. (command 0x01 - hello) */
  let response = await midi_send_and_receive(
    new Uint8Array([0xf0, 0x77, 0x01, 0xf7])
  );
  console.log("Firmware version response:", response);
  gemini_firmware_version = response.data[3];

  /* Update the info box with the firmware version. */
  connect_info.innerText = `Firmware version ${gemini_firmware_version}`;
  connect_info.classList.add("text-info");

  /* Now load settings. (command 0x08) */
  let settings_data = new Uint8Array(128);
  for (let n = 0; n < 4; n++) {
    let response = await midi_send_and_receive(
      new Uint8Array([0xf0, 0x77, 0x08, n, 0xf7])
    );
    for (let x = 0; x < 20; x++) {
      settings_data[20 * n + x] = response.data[3 + x];
    }
  }

  /* Update the form. */
  const decoded = teeth_decode(settings_data);
  const settings = GemSettings.unpack(decoded);
  console.log("Loaded settings", settings);
  form_from_settings(settings);
}

async function save_settings_to_device() {
  const settings = settings_from_form();
  console.log("Saving settings", settings);

  const settings_data = settings.pack();
  /* Always send 128 bytes. */
  let encoded_data = new Uint8Array(128);
  encoded_data.set(teeth_encode(settings_data));

  /* Send 20 bytes at a time. */
  for (let n = 0; n < 4; n++) {
    let midi_message = new Uint8Array(5 + 20);
    midi_message.set([0xf0, 0x77, 0x09, n]);
    for (let x = 0; x < 20; x++) {
      midi_message[4 + x] = encoded_data[20 * n + x];
    }
    midi_message[5 + 20 - 1] = 0xf7;
    await midi_send_and_receive(midi_message);
  }
}

connect_button.addEventListener("click", async function () {
  connect_info.classList.remove("text-danger", "text-success");
  connect_info.innerText = "Connecting";

  let access = await navigator.requestMIDIAccess({ sysex: true });

  for (const port of access.inputs.values()) {
    if (port.name === midi_port_name) {
      midi_input = port;
    }
  }
  for (const port of access.outputs.values()) {
    if (port.name === midi_port_name) {
      midi_output = port;
    }
  }

  if (midi_input === null || midi_output === null) {
    connect_info.classList.add("text-danger");
    connect_info.innerText =
      "Couldn't connect, check connection and power and try again?";
    return;
  }

  console.log(midi_input, midi_output);

  await load_settings_from_device();

  connect_button.disabled = true;
  connect_button.classList.remove("btn-primary");
  connect_button.classList.add("btn-success");
  connect_button.innerText = "Connected";
  connect_info.innerText = "";
  settings_form.classList.remove("hidden");
});

save_button.addEventListener("click", async function () {
  save_button.disabled = true;
  save_button.innerText = "Saving...";
  await save_settings_to_device();
  save_button.classList.remove("btn-primary");
  save_button.classList.add("btn-success");
  save_button.innerText = "Saved!";

  await new Promise((resolve) => setTimeout(resolve, 2000));

  save_button.classList.add("btn-primary");
  save_button.classList.remove("btn-success");
  save_button.innerText = "Save";
  save_button.disabled = false;
});

/*
    Validation logic. Hard limit all input[type="numbers"]
*/
function constrain_number_input(elem_name) {
  const input = settings_form[elem_name];
  const min = parseFloat(input.min);
  const max = parseFloat(input.max);

  input.addEventListener("change", function () {
    const value = input.valueAsNumber;
    if (value < min) {
      input.value = min;
    }
    if (value > max) {
      input.value = max;
    }
  });
}

constrain_number_input("castor_knob_min");
constrain_number_input("castor_knob_max");
constrain_number_input("pollux_knob_min");
constrain_number_input("pollux_knob_max");
constrain_number_input("cv_gain_error");
constrain_number_input("cv_offset_error");
constrain_number_input("adc_gain_corr");
constrain_number_input("adc_offset_corr");
constrain_number_input("pitch_knob_nonlinearity");

/*
    Enable/disable dangerous settings.
*/
allow_danger_input.addEventListener("change", function () {
  for (const elem of dangerous_fields) {
    if (elem.type === "range") {
      elem.disabled = !allow_danger_input.checked;
    } else {
      elem.readOnly = !allow_danger_input.checked;
    }
  }
});

/*
    Display interaction logic - updating display values for range inputs.
*/
function range_input_with_formatter(
  elem_name,
  formatter,
  postfix = "_display_value"
) {
  const input = settings_form[elem_name];
  const display_value = document.getElementById(`${elem_name}${postfix}`);

  function update() {
    display_value.innerText = formatter(input);
  }

  input.addEventListener("input", update);

  // Call it once to update it from the default value.
  update();
}

function range_input_with_percentage(elem_id) {
  range_input_with_formatter(elem_id, (input) => Math.round(input.value * 100));
}

function range_input_with_passthrough(elem_id) {
  range_input_with_formatter(elem_id, (input) => input.value);
}

range_input_with_percentage("chorus_max_intensity");
range_input_with_formatter("lfo_frequency", (input) =>
  input.valueAsNumber.toFixed(1)
);
range_input_with_percentage("smooth_initial_gain");
range_input_with_passthrough("smooth_sensitivity");
range_input_with_passthrough("pollux_follower_threshold");
range_input_with_formatter(
  "pollux_follower_threshold",
  (input) => ((input.valueAsNumber / 4096) * 6.0).toFixed(2),
  "_display_value_volts"
);
range_input_with_passthrough("cv_gain_error");
range_input_with_passthrough("cv_offset_error");
range_input_with_passthrough("adc_gain_corr");
range_input_with_passthrough("adc_offset_corr");
