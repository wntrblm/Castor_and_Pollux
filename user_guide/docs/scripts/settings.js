/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import { MIDI } from "/winter.js";
import { $e, $on } from "./utils.js";
import * as forms from "./forms.js";
import GemSettings from "./gem_settings.js";
import Gemini from "./gemini.js";
import GitHub from "./github.js";

const ui = {
    settings_form_elem: $e("settings_editor"),
    info_section: $e("info_section"),
    settings_section: $e("settings_section"),
    save_btn: $e("save_button"),
    allow_danger: $e("allow_danger"),
    danger_section: $e("danger_section"),
    dangerous_section_content: $e("danger_section_content"),
    connect_btn: $e("connect"),
    connect_info: $e("connect_info"),
    firmware_version: $e("firmware_version"),
    firmware_outdated: $e("firmware_outdated"),
    firmware_incompatible: $e("firmware_incompatible"),
    serial_number: $e("serial_number"),
    hardware_revision: $e("hardware_revision"),
    restore_adc_calibration_btn: $e("restore_adc_calibration"),
    lfo_waveform_canvas: $e("lfo-waveform-canvas"),
    tuning: {
        measure_one: $e("measure_one"),
        measure_one_result: $e("measure_one_result"),
        measure_three: $e("measure_three"),
        measure_three_result: $e("measure_three_result"),
        gain_error_result: $e("gain_error_result"),
        offset_error_result: $e("offset_error_result"),
        save: $e("save_tuning"),
    },
    monitoring: {
        section: $e("monitor_section"),
        enable: $e("enable_monitor"),
        output: $e("monitor_output"),
    },
    ramp: {
        section: $e("ramp_section"),
        swap_btn: $e("ramp_swap"),
        output: $e("ramp_output"),
    },
};

const midi = new MIDI("Gemini");
const gemini = new Gemini(midi);
const settings = new GemSettings();
let gemini_firmware_version = null;
let gemini_serial_number = null;
let gemini_hardware_revision = null;
let adc_calibration_backup = null;
let afe_calibration_backup = null;
let ramp_calibration_backup = null;

/*
  Factory calibrations are stored on GCS. This allows retrieving it.
*/

async function fetch_calibration(serial_no, type) {
    let response = await fetch(
        `https://raw.githubusercontent.com/wntrblm/calibrations/refs/heads/main/gemini/${serial_no}.${type}.json`
    );

    if (!response.ok) {
        console.warn(
            `Could not find ${type} calibration data for CPU ID ${serial_no}`
        );
        return null;
    }

    return await response.json();
}

async function check_for_backups() {
    adc_calibration_backup = await fetch_calibration(
        gemini_serial_number,
        "adc"
    );
    afe_calibration_backup = await fetch_calibration(
        gemini_serial_number,
        "afe"
    );
    ramp_calibration_backup = await fetch_calibration(
        gemini_serial_number,
        "ramp"
    );

    /* Check and see if there's a backup for  data. */
    if (!adc_calibration_backup && !afe_calibration_backup) {
        ui.restore_adc_calibration_btn.disabled = true;
        ui.restore_adc_calibration_btn.classList.remove("is-warning");
        ui.restore_adc_calibration_btn.classList.add("is-dark");
    }

    if (!ramp_calibration_backup) {
        ui.ramp.swap_btn.disabled = true;
        ui.ramp.swap_btn.classList.remove("is-warning");
        ui.ramp.swap_btn.classList.add("is-dark");
    }
}

async function restore_backup_calibration() {
    if (adc_calibration_backup) {
        settings.adc_gain_corr = Math.round(
            adc_calibration_backup.gain_error * 2048
        );
        settings.adc_offset_corr = Math.round(
            adc_calibration_backup.offset_error
        );
    }
    if (afe_calibration_backup) {
        settings.cv_gain_error = afe_calibration_backup.gain_error;
        settings.cv_offset_error = afe_calibration_backup.offset_error;
    }

    ui.settings_form.update();
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
        return false;
    }

    if (gemini_firmware_version.includes(release_info.tag_name)) {
        return false;
    }

    return true;
}

$on(ui.connect_btn, "click", async function () {
    const connect_info_p = ui.connect_info.querySelector("p");

    ui.connect_btn.innerText = "Connecting";
    ui.connect_btn.disabled = true;
    connect_info_p.innerText = "";
    ui.connect_info.hidden = true;

    try {
        await midi.connect();
    } catch (err) {
        console.log(err);
        connect_info_p.innerText =
            "Couldn't connect, check connection and power and try again?";
        ui.connect_info.hidden = false;
        return;
    } finally {
        ui.connect_btn.innerText = "Connect";
        ui.connect_btn.disabled = false;
    }

    gemini_firmware_version = await gemini.get_version();
    ui.firmware_version.value = `${gemini_firmware_version}`;

    const serial_num_result = await gemini.get_serial_number();
    gemini_serial_number = serial_num_result.serial;
    gemini_hardware_revision = serial_num_result.revision;
    ui.serial_number.value = `${gemini_serial_number}`;
    ui.hardware_revision.value = `v${gemini_hardware_revision}`;

    ui.info_section.hidden = false;

    if (await check_for_new_firmware()) {
        console.log("Firmware too old, bailing. :(");
        ui.firmware_incompatible.hidden = false;
        return;
    }

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
        connect_info_p.innerText =
            "Couldn't load settings, check connection, power, and try resetting the module.";
        return;
    }

    ui.settings_form.update();
    draw_lfo_waveform();

    ui.connect_btn.disabled = true;
    ui.connect_btn.innerText = "Connected";
    ui.connect_info.hidden = true;
    ui.settings_section.hidden = false;
    ui.danger_section.hidden = false;

    check_for_backups();
});

$on(ui.save_btn, "click", async function () {
    ui.save_btn.disabled = true;
    ui.save_btn.innerText = "Saving...";
    console.log("Saving settings", settings);

    await gemini.save_settings(settings);
    gemini.soft_reset();

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
    await restore_backup_calibration();
});

/*
    Enable/disable dangerous settings.
*/
$on(ui.allow_danger, "change", function () {
    ui.dangerous_section_content.hidden = !ui.allow_danger.checked;
});

/*
  Form data binding and display logic
*/
ui.settings_form = new forms.Form(ui.settings_form_elem, settings);

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

/*
    Tuning calibration helpers
*/
const tuning_info = {
    one_volt: 0,
    three_volt: 0,
    gain_error: 0,
    offset_error: 0,
};

$on(ui.tuning.measure_one, "click", async function () {
    tuning_info.one_volt = await gemini.read_adc_average(7, 20);
    ui.tuning.measure_one_result.value = tuning_info.one_volt;

    tuning_info.gain_error = null;
    tuning_info.offset_error = null;
    tuning_info.three_volt = null;

    ui.tuning.gain_error_result.value = tuning_info.gain_error;
    ui.tuning.offset_error_result.value = tuning_info.offset_error;
    ui.tuning.measure_three_result.value = tuning_info.three_volt;
    ui.tuning.save.disabled = true;
});

$on(ui.tuning.measure_three, "click", async function () {
    tuning_info.three_volt = await gemini.read_adc_average(7, 20);
    ui.tuning.measure_three_result.value = tuning_info.three_volt;

    if (!tuning_info.one_volt) return;

    const expected_low = gemini.volts_to_code(gemini_hardware_revision, 1.0);
    const expected_high = gemini.volts_to_code(gemini_hardware_revision, 3.0);
    console.log(expected_low, expected_high);
    const measured_low = tuning_info.one_volt;
    const measured_high = tuning_info.three_volt;
    tuning_info.gain_error = (
        (expected_high - expected_low) /
        (measured_high - measured_low)
    ).toFixed(4);
    tuning_info.offset_error = (
        measured_low * tuning_info.gain_error -
        expected_low
    ).toFixed(1);

    ui.tuning.gain_error_result.value = tuning_info.gain_error;
    ui.tuning.offset_error_result.value = tuning_info.offset_error;
    ui.tuning.save.disabled = false;
});

$on(ui.tuning.save, "click", function () {
    if (tuning_info.gain_error === null) return;
    if (tuning_info.offset_error === null) return;

    settings.cv_gain_error = tuning_info.gain_error;
    settings.cv_offset_error = tuning_info.offset_error;

    ui.settings_form.update();
    ui.save_btn.scrollIntoView();
    ui.save_btn.click();
});

/*
    Monitoring
*/
if (window.location.hash == "#monitor") {
    ui.monitoring.section.hidden = false;
}
$on(ui.monitoring.enable, "click", function () {
    gemini.enable_monitoring((msg) => {
        ui.monitoring.output.innerText = JSON.stringify(msg, undefined, 2);
    });
});

/*
    Ramp calibration swap
*/
if (window.location.hash == "#ramp") {
    ui.ramp.section.hidden = false;
}
$on(ui.ramp.swap_btn, "click", async function () {
    console.log(ramp_calibration_backup);

    const periods = Object.keys(ramp_calibration_backup["castor"])
        .sort((a, b) => parseInt(a, 10) - parseInt(b, 10))
        .reverse();

    for (let i = 0; i < periods.length; i++) {
        const period = periods[i];
        const period_number = parseInt(period, 10);
        let castor_value = ramp_calibration_backup["castor"][period];
        let pollux_value = ramp_calibration_backup["pollux"][period];

        // swap em
        [castor_value, pollux_value] = [pollux_value, castor_value];

        await gemini.write_lut_entry(
            i,
            period_number,
            castor_value,
            pollux_value
        );

        ui.ramp.output.innerText += `Set ${i}: period=${period}, castor=${pollux_value}, pollux=${castor_value}\n`;
    }

    await gemini.save_lut_table();

    ui.ramp.output.innerText += `Saved to NVM\n`;
});
