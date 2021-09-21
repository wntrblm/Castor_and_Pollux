/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import { $e, $on } from "./winterjs/utils.js";
import * as forms from "./winterjs/forms.js";
import MIDI from "./winterjs/midi.js";
import GemSettings from "./gem_settings.js";
import Gemini from "./gemini.js";
import GitHub from "./github.js";

const ui = {
    settings_form_elem: $e("settings_editor"),
    info_section: $e("info_section"),
    settings_section: $e("settings_section"),
    save_btn: $e("save_button"),
    dangerous_fields: [
        ...document.querySelectorAll("#settings_editor .is-dangerous"),
    ],
    allow_danger: $e("allow_danger"),
    connect_btn: $e("connect"),
    connect_info: $e("connect_info"),
    firmware_version: $e("firmware_version"),
    firmware_outdated: $e("firmware_outdated"),
    firmware_incompatible: $e("firmware_incompatible"),
    serial_number: $e("serial_number"),
    restore_adc_calibration_btn: $e("restore_adc_calibration"),
    lfo_waveform_canvas: $e("lfo-waveform-canvas"),
};

const midi = new MIDI("Gemini");
const gemini = new Gemini(midi);
const settings = new GemSettings();
/* The lowest compatible firmware version */
const minimum_firmware_version = new Date(2021, 7, 31);
let gemini_firmware_version = null;
let gemini_serial_number = null;

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

async function check_for_backups() {
    /* Check and see if there's a backup for the ADC data. */
    try {
        await fetch_calibration(gemini_serial_number, "adc");
    } catch {
        ui.restore_adc_calibration_btn.disabled = true;
        ui.restore_adc_calibration_btn.classList.remove("is-warning");
        ui.restore_adc_calibration_btn.classList.add("is-dark");
    }
}

async function restore_adc_calibration() {
    let adc_calibration = await fetch_calibration(gemini_serial_number, "adc");
    let afe_calibration = await fetch_calibration(gemini_serial_number, "afe");

    settings.adc_gain_corr = Math.round(adc_calibration.gain_error * 2048);
    settings.adc_offset_corr = Math.round(adc_calibration.offset_error);
    settings.cv_gain_error = afe_calibration.gain_error;
    settings.cv_offset_error = afe_calibration.offset_error;

    ui.settings_form.update();
}

function check_firmware_version() {
    let [year, month, day] = gemini_firmware_version
        .split(" ")[0]
        .split(".")
        .map((x) => parseInt(x, 10));

    let version_date = new Date(year, month - 1, day);

    console.log(year, month, day, version_date, minimum_firmware_version);

    if (version_date < minimum_firmware_version) {
        ui.firmware_incompatible.classList.remove("hidden");
        return false;
    }

    return true;
}

async function check_for_new_firmware() {
    let gh = new GitHub();
    let release_info = null;
    try {
        release_info = await gh.get_latest_release(
            "wntrblm",
            "Castor_and_Pollux"
        );
    } catch (e) {
        console.log("Error while fetching latest firmware: ", e);
        return;
    }

    if (gemini_firmware_version.includes(release_info.tag_name)) {
        return;
    }

    let link = ui.firmware_outdated.querySelector("a");
    link.href = release_info.html_url;
    link.innerText = `${release_info.name} (${release_info.tag_name})`;
    ui.firmware_outdated.classList.remove("hidden");
}

$on(ui.connect_btn, "click", async function () {
    ui.connect_info.classList.remove("is-danger", "hidden");
    ui.connect_info.innerText = "Connecting";

    try {
        await midi.connect();
    } catch (err) {
        console.log(err);
        ui.connect_info.classList.add("is-danger");
        ui.connect_info.innerText =
            "Couldn't connect, check connection and power and try again?";
        return;
    }

    gemini_firmware_version = await gemini.get_version();
    ui.firmware_version.value = `${gemini_firmware_version}`;
    gemini_serial_number = await gemini.get_serial_number();
    ui.serial_number.value = `${gemini_serial_number}`;

    ui.info_section.classList.remove("hidden");

    if (!check_firmware_version()) {
        console.log("Firmware too old, bailing. :(");
        return;
    }

    check_for_new_firmware();

    /* Load settings & update the form. */
    let loaded_settings = false;
    /* WebMIDI can sometimes inexplicably mess up SysEx messages, so try this a few times. */
    for (let attempt = 0; attempt < 10; attempt += 1) {
        try {
            Object.assign(settings, await gemini.load_settings());
            loaded_settings = true;
            console.log("Loaded settings", settings);
            break;
        } catch (err) {
            console.log("Retrying loading settings, got: ", err);
        }
    }

    if (!loaded_settings) {
        ui.connect_info.classList.add("is-danger");
        ui.connect_info.innerText =
            "Couldn't load settings, check connection, power, and try resetting the module.";
        return;
    }

    ui.settings_form.update();
    draw_lfo_waveform();

    ui.connect_btn.classList.remove("is-primary");
    ui.connect_btn.classList.add("is-success");
    ui.connect_btn.innerText = "Connected";
    ui.connect_info.classList.add("hidden");
    ui.connect_info.innerText = "";
    ui.settings_section.classList.remove("hidden");

    check_for_backups();
});

$on(ui.save_btn, "click", async function () {
    ui.save_btn.disabled = true;
    ui.save_btn.innerText = "Saving...";
    console.log("Saving settings", settings);

    await gemini.save_settings(settings);

    ui.save_btn.classList.remove("is-primary");
    ui.save_btn.classList.add("is-success");
    ui.save_btn.innerText = "Saved!";

    await new Promise((resolve) => setTimeout(resolve, 2000));

    ui.save_btn.classList.add("is-primary");
    ui.save_btn.classList.remove("is-success");
    ui.save_btn.innerText = "Save";
    ui.save_btn.disabled = false;
});

$on(ui.restore_adc_calibration_btn, "click", async function () {
    await restore_adc_calibration();
});

/*
    Enable/disable dangerous settings.
*/
$on(ui.allow_danger, "change", function () {
    for (const elem of ui.dangerous_fields) {
        if (elem.type === "range") {
            elem.disabled = !ui.allow_danger.checked;
        } else {
            elem.readOnly = !ui.allow_danger.checked;
        }
    }
});

/*
  Form data binding and display logic
*/
ui.settings_form = new forms.Form(ui.settings_form_elem, settings);
forms.bind_value_displays(ui.settings_form.elem);

new forms.ValueDisplay(
    ui.settings_form.elem["pollux_follower_threshold"],
    (input) => ((input.valueAsNumber / 4096) * 6.0).toFixed(2),
    "pollux_follower_threshold_value_display_volts"
);

/*
    Helper to draw the LFO waveform.
*/
function draw_lfo_waveform() {
    const get_waveform = (selection) => {
        selection = parseInt(selection);
        switch (selection) {
            // Triangle
            case 0:
                return (phase, frequency) =>
                    -1.0 + Math.abs(-2.0 + ((phase * frequency) % 1.0) * 4);
            // Sine
            case 1:
                return (phase, frequency) =>
                    Math.sin(2 * Math.PI * phase * frequency);
            // Sawtooth
            case 2:
                return (phase, frequency) =>
                    -1.0 + ((phase * frequency) % 1.0) * 2;
            // Square
            case 3:
                return (phase, frequency) =>
                    (phase * frequency) % 1.0 < 0.5 ? -1 : 1;
        }
    };

    const freq = 3;
    const lfo_1_f = get_waveform(settings.lfo_1_waveshape);
    const lfo_1_a = settings.lfo_1_factor;
    const lfo_2_f = get_waveform(settings.lfo_2_waveshape);
    const lfo_2_a = settings.lfo_2_factor;
    const lfo_2_r = settings.lfo_2_frequency_ratio;

    const lfo_func = (phase) => {
        return (
            lfo_1_f(phase, freq) * lfo_1_a +
            lfo_2_f(phase, freq * lfo_2_r) * lfo_2_a
        );
    };

    const canvas = ui.lfo_waveform_canvas;
    const ctx = canvas.getContext("2d");
    const padding = canvas.height * 0.2;

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    ctx.strokeStyle = "rgb(94, 64, 158)";
    ctx.lineWidth = 10;
    ctx.lineCap = "round";
    ctx.lineJoin = "round";

    ctx.beginPath();
    for (let i = 0; i < 1.1; i += 1 / canvas.width) {
        let val = lfo_func(0.25 + i);
        val = (val + 1) / 2;
        ctx.lineTo(
            i * canvas.width,
            padding + val * (canvas.height - padding * 2)
        );
    }
    ctx.stroke();
}

draw_lfo_waveform();
for (let input of ui.settings_form.elem.querySelectorAll("[name*=lfo]")) {
    input.addEventListener("input", () => {
        window.requestAnimationFrame(draw_lfo_waveform);
    });
}
