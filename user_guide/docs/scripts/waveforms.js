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
})();