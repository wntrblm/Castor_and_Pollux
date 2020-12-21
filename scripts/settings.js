import struct from "./struct.js";

const midi_port_name = "Gemini";
const settings_form = document.getElementById("settings_editor");
const save_button = document.getElementById("save_button");
const dangerous_fields = [...settings_form.querySelectorAll("input[readonly]"), ...settings_form.querySelectorAll("input[disabled]")];
const allow_danger_input = document.getElementById("allow_danger");
const connect_button = document.getElementById("connect");
const connect_info = document.getElementById("connect_info");
let gemini_firmware_version = null;

/* 16-bit fixed point helpers. */

const fix16_one = 0x00010000;

function to_fix16(val) {
    return Math.floor(val * fix16_one);
}

function from_fix16(val) {
    return val / fix16_one;
}

/*
    Serialization and deserialization of the settings.
*/

const settings_struct = struct(">BHhHiiiiiiiiiiH");
const settings_magic = 0x63;

function serialize_settings_form() {
    const form_data = new FormData(settings_form);

    let settings = [
        settings_magic,
        parseFloat(form_data.get("adc_gain_corr")),
        parseFloat(form_data.get("adc_offset_corr")),
        parseFloat(form_data.get("led_brightness")),
        parseFloat(to_fix16(form_data.get("castor_knob_min"))),
        parseFloat(to_fix16(form_data.get("castor_knob_max"))),
        parseFloat(to_fix16(form_data.get("pollux_knob_min"))),
        parseFloat(to_fix16(form_data.get("pollux_knob_max"))),
        parseFloat(to_fix16(form_data.get("chorus_max_intensity"))),
        parseFloat(to_fix16(form_data.get("chorus_frequency"))),
        parseFloat(to_fix16(form_data.get("knob_offset_corr"))),
        parseFloat(to_fix16(form_data.get("knob_gain_corr"))),
        parseFloat(to_fix16(form_data.get("smooth_initial_gain"))),
        parseFloat(to_fix16(form_data.get("smooth_sensitivity"))),
        parseFloat(form_data.get("pollux_follower_threshold")),
    ];

    const settings_data = new Uint8Array(settings_struct.pack(...settings));

    return settings_data;
}


function deserialize_settings_form(settings_data) {
    let settings = settings_struct.unpack(settings_data.buffer);

    console.log(settings);

    if(settings[0] !== settings_magic) {
        alert("Unable to load settings from device.");
    }

    settings_form.adc_gain_corr.value = settings[1];
    settings_form.adc_offset_corr.value = settings[2];
    settings_form.led_brightness.value = settings[3];
    settings_form.castor_knob_min.value = from_fix16(settings[4]).toFixed(2);
    settings_form.castor_knob_max.value = from_fix16(settings[5]).toFixed(2);
    settings_form.pollux_knob_min.value = from_fix16(settings[6]).toFixed(2);
    settings_form.pollux_knob_max.value = from_fix16(settings[7]).toFixed(2);
    settings_form.chorus_max_intensity.value = from_fix16(settings[8]).toFixed(2);
    settings_form.chorus_frequency.value = from_fix16(settings[9]).toFixed(1);
    settings_form.knob_offset_corr.value = from_fix16(settings[10]).toFixed(2);
    settings_form.knob_gain_corr.value = from_fix16(settings[11]).toFixed(2);
    settings_form.smooth_initial_gain.value = from_fix16(settings[12]).toFixed(2);
    settings_form.smooth_sensitivity.value = from_fix16(settings[13]).toFixed(1);
    settings_form.pollux_follower_threshold.value = settings[14];

    /* Trigger change event for inputs on the form. */
    for (const elem of settings_form.getElementsByTagName("input")) {
        elem.dispatchEvent(new Event('input'));
    }
}

/*
    WebMIDI interface.
*/

let midi_input = null;
let midi_output = null;

function midi_encode(src) {
    let result = new Uint8Array(src.length * 2);
    for(let n = 0; n < src.length; n++) {
        result[n * 2] = src[n] >> 4 & 0xF;
        result[n * 2 + 1] = src[n] & 0xF;
    }
    return result;
}

function midi_decode(src) {
    let result = new Uint8Array(src.length / 2);
    for(let n = 0; n < result.length; n++) {
        result[n] = src[n * 2] << 4 | src[n * 2 + 1];
    }
    return result;
}

async function midi_send_and_receive(data) {
    const done = new Promise(function (resolve, reject) {
        midi_input.onmidimessage = function(msg) {
            resolve(msg);
        }

        midi_output.send(data);
    });

    return await done;
}

async function load_settings_from_device() {
    /* First, get the firmware version. (command 0x01 - hello) */
    let response = await midi_send_and_receive(new Uint8Array([0xF0, 0x77, 0x01, 0xF7]));
    gemini_firmware_version = response.data[3];

    /* Update the info box with the firmware version. */
    connect_info.innerText = `Firmware version ${gemini_firmware_version}`;
    connect_info.classList.add("text-info");

    /* Now load settings. (command 0x08) */
    let settings_data = new Uint8Array(128);
    for(let n = 0; n < 8; n++) {
        let response = await midi_send_and_receive(new Uint8Array([0xF0, 0x77, 0x08, n, 0xF7]));
        for(let x = 0; x < 16; x++) {
            settings_data[16 * n + x] = response.data[3 + x];
        }
    }

    /* Update the form. */
    deserialize_settings_form(midi_decode(settings_data));
}

async function save_settings_to_device() {
    const settings_data = serialize_settings_form();
    /* Always send 128 bytes. */
    let encoded_data = new Uint8Array(128);
    encoded_data.set(midi_encode(settings_data));

    /* Send 16 bytes at a time. */
    for(let n = 0; n < 8; n++) {
        let midi_message = new Uint8Array(5 + 16);
        midi_message.set([0xF0, 0x77, 0x09, n]);
        for(let x = 0; x < 16; x++) {
            midi_message[4 + x] = encoded_data[16 * n + x];
        }
        midi_message[5 + 16 - 1] = 0xF7;
        await midi_send_and_receive(midi_message);
    }
}

connect_button.addEventListener("click", async function() {
    connect_info.classList.remove("text-danger", "text-success");
    connect_info.innerText = "Connecting";

    let access = await navigator.requestMIDIAccess({sysex: true});

    for(const port of access.inputs.values()) {
        if(port.name === midi_port_name) {
            midi_input = port;
        }
    }
    for(const port of access.outputs.values()) {
        if(port.name === midi_port_name) {
            midi_output = port;
        }
    }

    if(midi_input === null || midi_output === null) {
        connect_info.classList.add("text-danger");
        connect_info.innerText = "Couldn't connect, check connection and power and try again?";
        return;
    }

    console.log(midi_input, midi_output);

    connect_button.disabled = true;
    connect_button.classList.remove("btn-primary");
    connect_button.classList.add("btn-success");
    connect_button.innerText = "Connected";
    connect_info.innerText = "";
    settings_form.classList.remove("hidden");
    
    await load_settings_from_device();
});


save_button.addEventListener("click", async function() {
    save_button.disabled = true;
    save_button.innerText = "Saving...";
    await save_settings_to_device();
    save_button.classList.remove("btn-primary");
    save_button.classList.add("btn-success");
    save_button.innerText = "Saved!"

    await new Promise((resolve) => setTimeout(resolve, 2000));

    save_button.classList.add("btn-primary");
    save_button.classList.remove("btn-success");
    save_button.innerText = "Save"
    save_button.disabled = false;
});


/*
    Validation logic. Hard limit all input[type="numbers"]
*/
function constrain_number_input(elem_name) {
    const input = settings_form[elem_name];
    const min = parseFloat(input.min);
    const max = parseFloat(input.max);

    input.addEventListener('change', function() {
        const value = input.valueAsNumber;
        if(value < min) {
            input.value = min;
        }
        if(value > max) {
            input.value = max;
        }
    });
}

constrain_number_input("castor_knob_min");
constrain_number_input("castor_knob_max");
constrain_number_input("pollux_knob_min");
constrain_number_input("pollux_knob_max");
constrain_number_input("knob_gain_corr");
constrain_number_input("knob_offset_corr");
constrain_number_input("adc_gain_corr");
constrain_number_input("adc_offset_corr");

/*
    Enable/disable dangerous settings.
*/
allow_danger_input.addEventListener("change", function() {
    for(const elem of dangerous_fields) {
        if(elem.type === "range") {
            elem.disabled = !allow_danger_input.checked;
        } else {
            elem.readOnly = !allow_danger_input.checked;
        }
    }
});

/*
    Display interaction logic - updating display values for range inputs.
*/
function range_input_with_formatter(elem_name, formatter) {
    const input = settings_form[elem_name];
    const display_value = document.getElementById(elem_name + "_display_value");

    function update() {
        display_value.innerText = formatter(input);
    }

    input.addEventListener('input', update);

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
range_input_with_formatter("chorus_frequency", (input) => input.valueAsNumber.toFixed(1));
range_input_with_percentage("smooth_initial_gain");
range_input_with_passthrough("smooth_sensitivity");
range_input_with_passthrough("pollux_follower_threshold");
range_input_with_passthrough("knob_gain_corr");
range_input_with_passthrough("knob_offset_corr");
range_input_with_passthrough("adc_gain_corr");
range_input_with_passthrough("adc_offset_corr");