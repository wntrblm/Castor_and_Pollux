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

const ui = {
    settings_form: $e("settings_editor"),
    save_btn: $e("save_button"),
    dangerous_fields: [
        ...document.querySelectorAll("#settings_editor input[readonly]"),
        ...document.querySelectorAll("#settings_editor input[disabled]"),
    ],
    dangerous_fields_container: $e("danger_zone_form_controls"),
    allow_danger: $e("allow_danger"),
    connect_btn: $e("connect"),
    connect_info: $e("connect_info"),
    firmware_version: $e("firmware_version"),
    serial_number: $e("serial_number"),
    restore_adc_calibration_btn: $e("restore_adc_calibration"),
};

const midi = new MIDI("Gemini");
const gemini = new Gemini(midi);
const settings = new GemSettings();
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
        ui.restore_adc_calibration_btn.classList.remove("btn-warning");
        ui.restore_adc_calibration_btn.classList.add("btn-secondary");
    }
}

async function restore_adc_calibration() {
    let adc_calibration = await fetch_calibration(gemini_serial_number, "adc");
    let afe_calibration = await fetch_calibration(gemini_serial_number, "afe");

    settings.adc_gain_corr = Math.round(adc_calibration.gain_error * 2048);
    settings.adc_offset_corr = Math.round(adc_calibration.offset_error);
    settings.cv_gain_error = afe_calibration.gain_error;
    settings.cv_offset_error = afe_calibration.offset_error;

    forms.update_values(ui.settings_form);
}

$on(ui.connect_btn, "click", async function () {
    ui.connect_info.classList.remove("text-danger", "text-success");
    ui.connect_info.innerText = "Connecting";

    try {
        await midi.connect();
    } catch (err) {
        console.log(err);
        ui.connect_info.classList.add("text-danger");
        ui.connect_info.innerText =
            "Couldn't connect, check connection and power and try again?";
        return;
    }

    gemini_firmware_version = await gemini.get_version();
    ui.firmware_version.value = `${gemini_firmware_version}`;

    gemini_serial_number = await gemini.get_serial_number();
    ui.serial_number.value = `${gemini_serial_number}`;

    /* Load settings & update the form. */
    Object.assign(settings, await gemini.load_settings());
    console.log("Loaded settings", settings);

    forms.update_values(ui.settings_form);

    ui.connect_btn.disabled = true;
    ui.connect_btn.classList.remove("btn-primary");
    ui.connect_btn.classList.add("btn-success");
    ui.connect_btn.innerText = "Connected";
    ui.connect_info.innerText = "";
    ui.settings_form.classList.remove("hidden");

    check_for_backups();
});

$on(ui.save_btn, "click", async function () {
    ui.save_btn.disabled = true;
    ui.save_btn.innerText = "Saving...";
    console.log("Saving settings", settings);

    await gemini.save_settings(settings);

    ui.save_btn.classList.remove("btn-primary");
    ui.save_btn.classList.add("btn-success");
    ui.save_btn.innerText = "Saved!";

    await new Promise((resolve) => setTimeout(resolve, 2000));

    ui.save_btn.classList.add("btn-primary");
    ui.save_btn.classList.remove("btn-success");
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
    ui.dangerous_fields_container.classList.toggle("hidden");
});

/*
  Form data binding and display logic
*/
forms.bind(ui.settings_form, settings);
forms.bind_value_displays(ui.settings_form);

new forms.ValueDisplay(
    ui.settings_form["pollux_follower_threshold"],
    (input) => ((input.valueAsNumber / 4096) * 6.0).toFixed(2),
    "pollux_follower_threshold_value_display_volts"
);
