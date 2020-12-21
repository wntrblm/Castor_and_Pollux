(function(){
    "use strict";

    const no_audio_context = window.AudioContext === undefined;

    /* Share a single audiocontext and such between multiple
        instances. */
    
    let audio_ctx = null;
    const fft_size = 2048;
    let analyser = null;
    let buffer_length = null;
    let data_array = null;
    let current_source = null;

    if(!no_audio_context) {
        audio_ctx = new window.AudioContext();

        analyser = audio_ctx.createAnalyser();
        analyser.fftSize = fft_size;
        analyser.connect(audio_ctx.destination);

        buffer_length = analyser.frequencyBinCount;
        data_array = new Uint8Array(buffer_length);
    }

    function connect_media_element_source(source) {
        if (current_source !== null) {
            current_source.disconnect();
        }
        current_source = source;
        source.connect(analyser);
    }

    function winter_oscilloscope(wrapper) {
        const audio_elem = wrapper.querySelector("audio");
        let media_source = null;

        if(!no_audio_context) {
            media_source = audio_ctx.createMediaElementSource(audio_elem);
        }

        wrapper.classList.add("js");

        const canvas = document.createElement("canvas");
        canvas.width = 300;
        canvas.height = 50;
        wrapper.insertBefore(canvas, audio_elem);
    
        const canvas_ctx = canvas.getContext("2d");

        const title = document.createElement("p");
        title.innerText = audio_elem.title;
        wrapper.appendChild(title);

        function draw_analyzer() {
            analyser.getByteTimeDomainData(data_array);
            canvas_ctx.clearRect(0, 0, canvas.width, canvas.height);

            canvas_ctx.lineWidth = 2;
            canvas_ctx.strokeStyle = 'rgb(255, 255, 255)';
            canvas_ctx.beginPath();

            var slice_width = canvas.width * 1.0 / buffer_length;
            var x = 0;

            for(var i = 0; i < buffer_length; i++) {
                var v = data_array[i] / 128.0;
                var y = v * canvas.height/2;

                if(i === 0) {
                    canvas_ctx.moveTo(x, y);
                } else {
                    canvas_ctx.lineTo(x, y);
                }

                x += slice_width;
            }

            canvas_ctx.lineTo(canvas.width, canvas.height / 2);
            canvas_ctx.stroke();
            
            if(!audio_elem.paused && !audio_elem.ended) {
                window.requestAnimationFrame(draw);
            }
        }

        let sine_offset = 0;

        function draw_sine(once) {
            canvas_ctx.clearRect(0, 0, canvas.width, canvas.height);
            canvas_ctx.lineWidth = 2;
            canvas_ctx.strokeStyle = 'rgb(255, 255, 255)';
            canvas_ctx.beginPath();

            const divider = 4;
            const width = parseFloat(canvas.width) / divider;

            for(var i = 0; i < width + 1; i++) {
                var v = Math.sin(8 * Math.PI * (i / width) + (sine_offset / 8));
                var y = canvas.height / 2 + (v * canvas.height / 2 * 0.9);
                if(i === 0) {
                    canvas_ctx.moveTo(i * divider, y);
                } else {
                    canvas_ctx.lineTo(i * divider, y);
                }
            }

            canvas_ctx.stroke();

            sine_offset++;

            if(!once && !audio_elem.paused && !audio_elem.ended) {
                window.requestAnimationFrame(draw);
            }
        }

        function draw() {
            if(no_audio_context) {
                draw_sine();
            } else {
                draw_analyzer();
            }
        }

        audio_elem.addEventListener("play", function(e) {
            /* pause all other elements. */
            document.querySelectorAll(".winter-oscilloscope audio").forEach((el) => {
                if(el !== audio_elem) el.pause(); 
            });

            if(!no_audio_context) {
                connect_media_element_source(media_source);
                audio_ctx.resume();
            }

            draw();
        });

        canvas.addEventListener("click", function() {
            if(audio_elem.paused) {
                audio_elem.play();
            } else {
                audio_elem.pause();
            }
        });

        draw_sine(true);
    }

    document.querySelectorAll(".winter-oscilloscope").forEach((el) => winter_oscilloscope(el));
    
})();