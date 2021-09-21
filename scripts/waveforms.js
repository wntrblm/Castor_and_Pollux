/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import { $e, $on } from "./winterjs/utils.js";
import * as forms from "./winterjs/forms.js";

/* Global parameters for every waveform display. */
const frequency = 3.0;
const speed = 0.02;

class WaveformDisplay {
    constructor(canvas, fn) {
        this.canvas = $e(canvas);
        this.ctx = this.canvas.getContext("2d");
        this.fn = fn;
        this.playing = false;
        this.t = 0;

        $on(this.canvas, "click", () => this.play_pause());

        this.draw(true);
    }

    play_pause() {
        if (!this.playing) {
            this.playing = true;
            window.requestAnimationFrame(() => this.draw(false));
        } else {
            this.playing = false;
        }
    }

    update() {
        if (this.playing) return;
        this.draw(true);
    }

    draw(one_frame) {
        const resolution = this.canvas.width;
        const padding = 100;

        if (one_frame !== true) {
            this.t += speed;
        }

        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);

        this.ctx.strokeStyle = "#FFF";
        this.ctx.lineWidth = 10;
        this.ctx.lineCap = "round";
        this.ctx.lineJoin = "round";

        this.ctx.beginPath();

        for (let i = 0; i < 1.1; i += 1 / resolution) {
            let val = this.fn(this.t, i);
            this.ctx.lineTo(
                i * this.canvas.width,
                padding + val * (this.canvas.height - padding * 2)
            );
        }

        this.ctx.stroke();

        this.ctx.fillStyle = "#fff";
        this.ctx.font = "20px Interstate, san-serif";

        if (this.playing) {
            this.ctx.fillText("❚❚", 20, 38);
        } else {
            this.ctx.fillText("▶\uFE0E", 20, 40);
        }

        if (this.playing) {
            window.requestAnimationFrame(() => this.draw(false));
        }
    }
}

/* Ramp waveform. */

function ramp_waveform(phase, offset) {
    return (phase + offset * frequency) % 1.0;
}

new WaveformDisplay("ramp", ramp_waveform);

/* Pulse waveform. */

function pulse_waveform(phase, offset, duty) {
    phase = (phase + offset * frequency) % 1.0;
    return phase < duty ? 0 : 1;
}

const pulse_config = {
    width: 0.5,
};

const pulse_display = new WaveformDisplay($e("pulse"), (phase, offset) => {
    return pulse_waveform(phase, offset, pulse_config.width);
});

new forms.Form($e("pulse-form"), pulse_config);
$on($e("pulse-form"), "input", () => pulse_display.update());

/* Sub waveform */

new WaveformDisplay("sub", (phase, offset) =>
    pulse_waveform(phase / 2, offset / 2, 0.5)
);

/* Mix waveform */

function mix_waveform(
    phase,
    offset,
    ramp_mix,
    pulse_mix,
    sub_mix,
    pulse_width
) {
    let value = 0.5;
    value += (ramp_waveform(phase, offset) - 0.5) * ramp_mix;
    value += (pulse_waveform(phase, offset, pulse_width) - 0.5) * pulse_mix;
    value += (pulse_waveform(phase / 2, offset / 2, 0.5) - 0.5) * sub_mix;
    return value;
}

const mix_config = {
    ramp_mix: 0.5,
    pulse_mix: 0.25,
    sub_mix: 0.25,
    pulse_width: 0.5,
};

const mix_display = new WaveformDisplay($e("mix"), (phase, offset) => {
    return mix_waveform(
        phase,
        offset,
        mix_config.ramp_mix,
        mix_config.pulse_mix,
        mix_config.sub_mix,
        mix_config.pulse_width
    );
});

new forms.Form($e("mix-form"), mix_config);
$on($e("mix-form"), "input", () => mix_display.update());

/* Stacked oscillators. */

function stacked_waveform(phase, offset, mix, detune) {
    let rampa = (phase + offset * frequency) % 1.0;
    let rampb = (phase + offset * (frequency + detune)) % 1.0;
    return rampa * (1.0 - mix) + rampb * mix;
}

const stacked_config = {
    mix: 0.5,
    detune: 0.25,
};

const stacked_display = new WaveformDisplay($e("stacked"), (phase, offset) => {
    return stacked_waveform(
        phase,
        offset,
        stacked_config.mix,
        stacked_config.detune
    );
});

new forms.Form($e("stacked-form"), stacked_config);
$on($e("stacked-form"), "input", () => stacked_display.update());

/* Chorusing */
function triangle_waveform(phase, offset, frequency) {
    return Math.abs(((phase * frequency + offset) % 1.0) - 0.5) * 2 - 0.5;
}

function chorusing_waveform(phase, offset, amount) {
    let mix = 0.5;
    let fm = triangle_waveform(phase, 0, 0.2) * amount;
    let rampa = (phase + offset * frequency) % 1.0;
    let rampb = (phase + (offset * frequency + fm)) % 1.0;
    return rampa * (1.0 - mix) + rampb * mix;
}

const chorusing_config = {
    amount: 0.25,
};

const chorusing_display = new WaveformDisplay(
    $e("chorusing_"),
    (phase, offset) => {
        return chorusing_waveform(phase, offset, chorusing_config.amount);
    }
);

new forms.Form($e("chorusing-form"), chorusing_config);
$on($e("chorusing-form"), "input", () => chorusing_display.update());

/* Non-linear tune */

const tuning_slider = document.getElementById("tuning_slider");
const linear_output = document.getElementById("linear_tune");
const non_linear_output = document.getElementById("non_linear_tune");
const linear_tune_canvas = document.getElementById("linear_tune_canvas");
const nonlinear_tune_canvas = document.getElementById("nonlinear_tune_canvas");

function bezier_1d_2c(c1, c2, t) {
    /* Copied from gem_bezier.c */
    return (
        3 * c1 * t * Math.pow(1 - t, 2) +
        3 * c2 * (1 - t) * Math.pow(t, 2) +
        Math.pow(t, 3)
    );
}

function update_tuning() {
    const tuning = tuning_slider.valueAsNumber;
    const linear_response = 440 * Math.pow(2, tuning);
    const non_linear_tuning =
        -1.0 + bezier_1d_2c(0.6, 1.0 - 0.6, (tuning + 1.0) / 2) * 2;
    const non_linear_response = 440 * Math.pow(2, non_linear_tuning);

    linear_output.innerText = `${linear_response.toFixed(2)} Hz`;
    non_linear_output.innerText = `${non_linear_response.toFixed(2)} Hz`;

    window.requestAnimationFrame(() =>
        draw_tuning_thingy(
            linear_tune_canvas,
            "125, 97, 186",
            (x) => x,
            (tuning + 1.0) / 2
        )
    );
    window.requestAnimationFrame(() =>
        draw_tuning_thingy(
            nonlinear_tune_canvas,
            "64, 140, 148",
            (x) => bezier_1d_2c(0.6, 1.0 - 0.6, x),
            (non_linear_tuning + 1.0) / 2
        )
    );
}

tuning_slider.addEventListener("input", function () {
    update_tuning();
});

update_tuning();

function draw_tuning_thingy(canvas, color, func, selected) {
    const ctx = canvas.getContext("2d");
    const iterations = 20;
    const radius = canvas.height * 0.4;
    const padding = radius * 1.3;

    ctx.lineWidth = 2;
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (let n = 0; n < iterations + 1; n++) {
        const progress = n / iterations;
        const output = func(progress);
        const alpha = Math.max(0.2, 1.0 - Math.abs(progress - selected) * 5);
        ctx.strokeStyle = `rgba(${color}, ${alpha})`;
        ctx.beginPath();
        ctx.arc(
            padding + output * (canvas.width - padding * 2),
            canvas.height / 2,
            radius,
            0,
            2 * Math.PI
        );
        ctx.stroke();
    }
}
