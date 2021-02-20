import GemSettings from "./gem_settings.js";
import { teeth_decode, teeth_encode, teeth_encoded_length } from "./teeth.js";

const midi_port_name = "Gemini";
const settings_chunk_size = 10;
const settings_chunk_count =
  teeth_encoded_length(GemSettings.packed_size) / settings_chunk_size;
const settings_form = document.getElementById("settings_editor");
const save_button = document.getElementById("save_button");
const dangerous_fields = [
  ...settings_form.querySelectorAll("input[readonly]"),
  ...settings_form.querySelectorAll("input[disabled]"),
];
const allow_danger_input = document.getElementById("allow_danger");
const danger_zone_form_controls = document.getElementById(
  "danger_zone_form_controls"
);
const connect_button = document.getElementById("connect");
const connect_info = document.getElementById("connect_info");
const firmware_version_info = document.getElementById("firmware_version");
const restore_adc_calibration_button = document.getElementById(
  "restore_adc_calibration"
);
let gemini_firmware_version = null;
let gemini_serial_number = null;

/*
  Helper functions
*/

function Uint8Array_to_hex(buf) {
  return Array.prototype.map
    .call(buf, (x) => ("00" + x.toString(16)).slice(-2))
    .join("");
}

/*
  Form loading/saving
*/

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
  settings_form.cv_gain_error.value = settings.cv_gain_error.toFixed(4);
  settings_form.smooth_initial_gain.value = settings.smooth_initial_gain.toFixed(
    2
  );
  settings_form.smooth_sensitivity.value = settings.smooth_sensitivity.toFixed(
    4
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
  Factory calibrations are stored on GCS. This allows retrieving it.
*/

async function fetch_calibration(serial_no, type) {
  let response = await fetch(
    `https://storage.googleapis.com/files.winterbloom.com/calibrations/gemini/${serial_no}.${type}.json`
  );

  if (!response.ok) {
    throw `Could not find ${type} calibration data for CPU ID ${serial_no}`;
  }

  let calibration_data = await response.json();

  return calibration_data;
}

async function restore_adc_calibration() {
  let settings = settings_from_form();
  let adc_calibration = await fetch_calibration(gemini_serial_number, "adc");
  let afe_calibration = await fetch_calibration(gemini_serial_number, "afe");

  settings.adc_gain_corr = Math.round(adc_calibration.gain_error * 2048);
  settings.adc_offset_corr = Math.round(adc_calibration.offset_error);
  settings.cv_gain_error = afe_calibration.gain_error;
  settings.cv_offset_error = afe_calibration.offset_error;

  form_from_settings(settings);
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
  gemini_firmware_version = new TextDecoder("ascii").decode(
    response.data.slice(3, -1)
  );

  /* Update the info box with the firmware version. */
  firmware_version_info.innerText = `Firmware version ${gemini_firmware_version}`;

  /* Get the CPU ID/Serial number. (command 0x0F) */
  response = await midi_send_and_receive(
    new Uint8Array([0xf0, 0x77, 0x0f, 0xf7])
  );
  gemini_serial_number = Uint8Array_to_hex(
    teeth_decode(response.data.slice(3, -1))
  );
  console.log("Serial number:", gemini_serial_number);

  /* Now load settings. (command 0x08) */
  let settings_data = new Uint8Array(
    teeth_encoded_length(GemSettings.packed_size)
  );
  for (let n = 0; n < settings_chunk_count; n++) {
    let response = await midi_send_and_receive(
      new Uint8Array([0xf0, 0x77, 0x08, n, 0xf7])
    );

    if (response.data.length != settings_chunk_size + 4) {
      throw `Invalid settings chunk data! ${response.data}`;
    }

    for (let x = 0; x < settings_chunk_size; x++) {
      settings_data[settings_chunk_size * n + x] = response.data[3 + x];
    }
  }

  /* Update the form. */
  const decoded = teeth_decode(settings_data);
  const settings = GemSettings.unpack(decoded);
  console.log("Loaded settings", settings);
  form_from_settings(settings);

  /* Check and see if there's a backup for the ADC data. */
  try {
    await fetch_calibration(gemini_serial_number, "adc");
  } catch {
    restore_adc_calibration_button.disabled = true;
    restore_adc_calibration_button.classList.remove("btn-warning");
    restore_adc_calibration_button.classList.add("btn-secondary");
  }
}

async function save_settings_to_device() {
  const settings = settings_from_form();
  console.log("Saving settings", settings);

  const settings_data = settings.pack();
  /* Always send 128 bytes. */
  let encoded_data = new Uint8Array(
    teeth_encoded_length(GemSettings.packed_size)
  );
  encoded_data.set(teeth_encode(settings_data));

  /* Send one chunk at a time. */
  for (let n = 0; n < settings_chunk_count; n++) {
    let midi_message = new Uint8Array(5 + settings_chunk_size);
    midi_message.set([0xf0, 0x77, 0x09, n]);
    for (let x = 0; x < settings_chunk_size; x++) {
      midi_message[4 + x] = encoded_data[settings_chunk_size * n + x];
    }
    midi_message[5 + settings_chunk_size - 1] = 0xf7;
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

restore_adc_calibration_button.addEventListener("click", async function () {
  await restore_adc_calibration();
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
  danger_zone_form_controls.classList.toggle("hidden");
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
range_input_with_percentage("pitch_knob_nonlinearity");
range_input_with_percentage("smooth_initial_gain");
range_input_with_passthrough("smooth_sensitivity");
range_input_with_passthrough("pollux_follower_threshold");
range_input_with_formatter(
  "pollux_follower_threshold",
  (input) => ((input.valueAsNumber / 4096) * 6.0).toFixed(2),
  "_display_value_volts"
);
range_input_with_formatter("adc_gain_corr", (input) =>
  (input.valueAsNumber / 2048).toFixed(3)
);
range_input_with_passthrough("adc_offset_corr");
