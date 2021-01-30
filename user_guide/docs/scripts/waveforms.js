(function(){
    'use strict';

    /* Global parameters for every waveform display. */
    const frequency = 3.0;
    const speed = 0.02;

    /* This function actually draws and updates the canvas. */
    function attachWaveform(canvas_id, waveform_fn) {
        const canvas = document.getElementById(canvas_id);
        const ctx = canvas.getContext('2d');
        let playing = false;
        let t = 0;

        function draw(one_frame) {
            if(one_frame === undefined || one_frame === false) {
                t += speed;
            }

            const resolution = canvas.width;

            ctx.clearRect(0, 0, canvas.width, canvas.height);

            ctx.strokeStyle = '#FFF';
            ctx.lineWidth = 10;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";

            let padding = ctx.lineWidth * 10;

            ctx.beginPath();

            for(let i = 0; i < 1.1; i += 1/resolution) {
                let val = waveform_fn(t, i);
                ctx.lineTo(i * canvas.width, padding + (val * (canvas.height - padding * 2)));
            }

            ctx.stroke();

            ctx.fillStyle = '#fff';
            ctx.font = '20px Interstate, san-serif';
            if(playing) {
                ctx.fillText('❚❚', 20, 38);
            } else {
                ctx.fillText('▶\uFE0E', 20, 40);
            }

            if(playing) {
                window.requestAnimationFrame(() => draw(false));
            }
        }

        window.requestAnimationFrame(() => draw(false));

        canvas.addEventListener('click', function(event) {
            if(!playing) {
                playing = true;
                window.requestAnimationFrame(() => draw(false));
            } else {
                playing = false;
            }
        });

        return function(one_frame) {
            if (playing) {
                return;
            }
            window.requestAnimationFrame(() => draw(one_frame));
        };
    }

    /* Ramp waveform. */

    function ramp_waveform(phase, offset) {
        return (phase + (offset * frequency)) % 1.0;
    }

    attachWaveform('ramp', ramp_waveform);

    /* Pulse waveform. */

    function pulse_waveform(phase, offset, duty) {
        phase = (phase + (offset * frequency)) % 1.0;
        return phase < duty ? 0 : 1;
    }

    let pulse_width = 0.5;

    const update_pulse = attachWaveform('pulse', (phase, offset) => pulse_waveform(phase, offset, pulse_width));

    const pulse_width_slider = document.getElementById('pulse_width');
    pulse_width_slider.addEventListener('input', function(e) {
        pulse_width = pulse_width_slider.valueAsNumber;
        update_pulse(true);
    });

    /* Sub waveform */
    attachWaveform('sub', (phase, offset) => pulse_waveform(phase / 2, offset / 2, 0.5));


    /* Mix waveform */

    function mix_waveform(phase, offset, ramp_mix, pulse_mix, sub_mix, pulse_width) {
        let value = 0.5;
        value += (ramp_waveform(phase, offset) - 0.5) * ramp_mix;
        value += (pulse_waveform(phase, offset, pulse_width) - 0.5) * pulse_mix;
        value += (pulse_waveform(phase / 2, offset / 2, 0.5) - 0.5) * sub_mix;
        return value;
    }

    const ramp_mix_slider = document.getElementById('ramp_mix');
    const pulse_mix_slider = document.getElementById('pulse_mix');
    const sub_mix_slider = document.getElementById('sub_mix');
    const pulse_width_mix_slider = document.getElementById('pulse_width_mix');
    let ramp_mix = ramp_mix_slider.valueAsNumber;
    let pulse_mix = pulse_mix_slider.valueAsNumber;
    let sub_mix = sub_mix_slider.valueAsNumber;
    let pulse_width_mix = pulse_width_mix_slider.valueAsNumber;

    const update_mix = attachWaveform('mix', (phase, offset) => mix_waveform(phase, offset, ramp_mix, pulse_mix, sub_mix, pulse_width_mix));

    ramp_mix_slider.addEventListener('input', function(e) {
        ramp_mix = ramp_mix_slider.valueAsNumber;
        update_mix(true);
    });
    pulse_mix_slider.addEventListener('input', function(e) {
        pulse_mix = pulse_mix_slider.valueAsNumber;
        update_mix(true);
    });
    sub_mix_slider.addEventListener('input', function(e) {
        sub_mix = sub_mix_slider.valueAsNumber;
        update_mix(true);
    });
    pulse_width_mix_slider.addEventListener('input', function(e) {
        pulse_width_mix = pulse_width_mix_slider.valueAsNumber;
        update_mix(true);
    });

    /* Stacked oscillators. */

    function stacked_waveform(phase, offset, mix, detune) {
        let rampa = (phase + (offset * frequency)) % 1.0;
        let rampb = (phase + (offset * (frequency + detune))) % 1.0;
        return (rampa * (1.0 - mix)) + (rampb * mix);
    }

    const stacked_mix_slider = document.getElementById('stacked_mix');
    const stacked_detune_slider = document.getElementById('stacked_detune');
    let stacked_mix = stacked_mix_slider.valueAsNumber;
    let stacked_detune = stacked_detune_slider.valueAsNumber;

    const update_stacked = attachWaveform('stacked', (phase, offset) => stacked_waveform(phase, offset, stacked_mix, stacked_detune));

    stacked_mix_slider.addEventListener('input', function(e) {
        stacked_mix = stacked_mix_slider.valueAsNumber;
        update_stacked(true);
    });
    stacked_detune_slider.addEventListener('input', function(e) {
        stacked_detune = stacked_detune_slider.valueAsNumber;
        update_stacked(true);
    });

    /* Chorusing */
    function triangle_waveform(phase, offset, frequency) {
        return Math.abs(((phase * frequency + offset) % 1.0) - 0.5) * 2 - 0.5;
    }

    function chorusing_waveform(phase, offset, amount) {
        let mix = 0.5;
        let fm = triangle_waveform(phase, 0, 0.2) * amount;
        let rampa = (phase + (offset * frequency)) % 1.0;
        let rampb = (phase + (offset * (frequency) + fm)) % 1.0;
        return (rampa * (1.0 - mix)) + (rampb * mix);
    }

    const chorusing_amount_slider = document.getElementById('chorusing_amount');
    let chorusing_amount = chorusing_amount_slider.valueAsNumber;

    const update_chorusing = attachWaveform('chorusing_', (phase, offset) => chorusing_waveform(phase, offset, chorusing_amount));

    chorusing_amount_slider.addEventListener('input', function(e) {
        chorusing_amount = chorusing_amount_slider.valueAsNumber;
        update_chorusing(true);
    });

    /* Non-linear tune */
    const tuning_slider = document.getElementById('tuning_slider');
    const linear_output = document.getElementById('linear_tune');
    const non_linear_output = document.getElementById('non_linear_tune');
    const difference_output = document.getElementById('tuning_difference');
    const linear_tune_canvas = document.getElementById('linear_tune_canvas');

    function bezier_1d_2c(c1, c2, t) {
        /* Copied from gem_bezier.c */
        return 3 * c1 * t * Math.pow(1 - t, 2) + 3 * c2 * (1 - t) * Math.pow(t, 2) + Math.pow(t, 3);
    }

    function update_tuning() {
        const tuning = tuning_slider.valueAsNumber;
        const linear_response = 440 * Math.pow(2, tuning);
        const non_linear_tuning = -1.0 + bezier_1d_2c(0.6, 1.0 - 0.6, (tuning + 1.0) / 2) * 2;
        const non_linear_response = 440 * Math.pow(2, non_linear_tuning);

        linear_output.innerText = `${linear_response.toFixed(2)} Hz`;
        non_linear_output.innerText = `${non_linear_response.toFixed(2)} Hz`;

        window.requestAnimationFrame(() => draw_tuning_thingy(linear_tune_canvas, '125, 97, 186', (x) => x, (tuning + 1.0) / 2));
        window.requestAnimationFrame(() => draw_tuning_thingy(nonlinear_tune_canvas, '64, 140, 148', (x) => bezier_1d_2c(0.6, 1.0 - 0.6, x), (non_linear_tuning + 1.0) / 2));
    }

    tuning_slider.addEventListener('input', function(e) {
        update_tuning();
    });

    update_tuning();

    function draw_tuning_thingy(canvas, color, func, selected) {
        const ctx = canvas.getContext('2d');
        const iterations = 20;
        const radius = canvas.height * 0.4;
        const padding = radius * 1.3;

        ctx.lineWidth = 2;
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        for(let n = 0; n < iterations + 1; n++) {
            const progress = n / iterations;
            const output = func(progress);
            const alpha = Math.max(0.2, (1.0 - Math.abs(progress - selected) * 5));
            ctx.strokeStyle = `rgba(${color}, ${alpha})`;
            ctx.beginPath();
            ctx.arc(padding + (output * (canvas.width - padding * 2)), canvas.height / 2, radius, 0, 2 * Math.PI);
            ctx.stroke();
        }
    }
})();
