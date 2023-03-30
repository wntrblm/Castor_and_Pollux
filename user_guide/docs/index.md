# Castor & Pollux User Guide

[Castor & Pollux](https://winterbloom.com/store/winterbloom-castor-and-pollux) is a Roland Juno-inspired oscillator. It contains two digitally-controlled analog oscillators implemented using a faithfully modernized version of the original Juno 106 design. Like its inspiration, it can give you that wonderful 80s sound with independent and mixed ramp, pulse, and sub outputs. But the twins do not stop there- the second oscillator can be used to layer in thick detuned sounds, harmonically complex hard synced waveshapes, or as a fully independent second oscillator.

[TOC]

## Specifications

-   Available [assembled](https://winterbloom.com/shop/castor-and-pollux) or as a [kit](https://winterbloom.com/shop/castor-and-pollux-kit)
-   14 HP
-   `+12 V @ 100 mA`, `-12 V @ 35 mA`
-   `25 mm` deep
-   2 separate oscillators
-   7 octave range
-   9 total outputs with waveform outputs and onboard mixers
-   Pitch CV input jacks (`0 V` to `6 V`) and pitch knobs (`-1` to `+1 octave`, configurable)
-   Pulse-width CV input jacks and knobs
-   Hard sync
-   Hardware chorusing
-   Internal low-frequency oscillator
-   [ModularGrid page](https://www.modulargrid.net/e/winterbloom-castor-pollux)

## Getting support and help

We want you to have a wonderful experience with your module. If you need help or run into problems, please reach out to us. Email is the best way for product issues, whereas Discord and GitHub are great for getting advice and help on how to customize your module or create new patches.

-   Send us [an email](mailto:support@winterbloom.com)
-   File a bug [on GitHub](https://github.com/wntrblm/Castor_and_Pollux/issues)
-   Reach out on our [Discord][discord]

## Version differences

![Illustration of Castor & Pollux I and Castor & Pollux II](images/V1%20vs%20V2.svg){.dark-invert}

Castor & Pollux has two different versions in the wild with significant changes between them. Fortunately, both versions are fundamentally the same brains with different user interfaces. Changes made by Castor & Pollux II include:

-   Replacing the pitch trimpots with large knobs
-   Replacing the pulse width, lfo, and crossfade trimpots with large knobs
-   Moving the input and output jacks to the bottom edge of the module
-   Moving the individual waveform outputs to a seperate expander
-   Consolidating the two separate circuit boards into a single board
-   Adding additional ESD protection to all input jacks
-   Adding an additional LED

Both versions use the same firmware and behave the same way. This manual applies to both versions, however, the illustrations shown here depict Castor & Pollux II. We made sure that all interface elements use the same symbols and labels between the versions. If you run into trouble, don't hesitate to [reach out](#getting-support-and-help).

## Installation

![Illustration of power connection](images/Power%20connection.svg){.dark-invert}

To install this into your Eurorack setup, connect a Eurorack power cable from your power supply to the back of the module. **Note that even though the power connector on the module is keyed, double check that the red stripe is on the side labeled `red stripe`!**. Once power is connected, secure your module to your rack rails using screws.

## Overview

Castor & Pollux is has two separate but _intertwined_ oscillators. It's possible to control each oscillator independently, but they truly shine when used together. Castor & Pollux's design is focused around the connection between these two oscillators and modulating their parameters using the internal LFO.

![Illustration of Castor & Pollux's interface with elements for the separate oscillators highlighted](images/1%20-%20Overview.svg){.dark-invert}

Castor & Pollux's front panel is arranged so that Castor's inputs, outputs, and controls are on the **left** whereas the corresponding elements are mirrored on the **right** for Pollux. The center contains controls for the internal LFO and crossfade mixer:

Each oscillator, `⍺` and `β`, is controlled by its associated knobs and CV input jacks:

![Illustration of the knobs and input jacks](images/2%20-%20Osc%20controls.svg){.dark-invert}

-   The large pitch knob and pitch CV input control the oscillator's **pitch** (frequency).
-   The smaller pulse width knob and pulse width CV input control the oscillator's **pulse width**.

You can learn more about these inputs in the [pitch behavior](#pitch-behavior) section.

Each oscillator has a single output jack, `★`, which is controlled by its mixer:

![Illustration of the oscillator's outputs](images/3%20-%20Osc%20outputs.svg){.dark-invert}

Each oscillator generates three waveforms: _saw_, _pulse_, and _sub_. The three mixing knobs control how much of each waveform is present in the oscillator's output. You can read more about the sound of each waveshape in the [waveshapes](#waveshapes) section.

Next, the output of the two oscillators is combined together by the _crossfade mixer_:

![Illustration of the crossfade mixer and its output jack](images/4%20-%20Crossfader.svg){.dark-invert}

The crossfader's knob, `Σ`, determines which oscillator is more prominent in the mix at the crossfade output, `♊️`. The [oscillator stacking](#oscillator-stacking) section has more details and sound samples of combining the oscillators together.

Next up, in the very middle there's a the LFO knob, `φ`:

![Illustration of the LFO knob](images/5%20-%20LFO.svg){.dark-invert}

This [internal LFO](#internal-low-frequency-oscillator) can be used to modulate several parameters in interesting ways depending on the [mode](#modes--tweaking).

Last, but not least, there is a single button in the center that's used for changing modes and enabling the tweak overlay:

![Illustration of the button](images/6%20-%20Button.svg){.dark-invert}

You can learn more about the different modes and tweaking in the [modes & tweaking section](#modes--tweaking).

## First steps

If you're not sure where to start with Castor & Pollux this section has a few patches to get you up and running.

### Patch one

First, dip your toes in by putting together this patch:

![Illustration of the first patch](images/7%20-%20Patch%201.svg){.dark-invert}

-   Turn all the knobs and trimpots fully counter-clockwise
-   Turn the ramp mix trimpot for Castor fully clockwise
-   Patch Castor's output jack to your rack's output or headphones module

You should hear a low, rumbly bass sound. Turn Castor's pitch knob (the big knob at the top left) and you'll hear Castor sweep through its range. This is called _coarse_ behavior and it's explained in more detail in the [pitch](#pitch) section.

You can keep playing around with this patch by moving the mix trimpots for each of the waveshapes to hear how they sound. You can also jump down to the [waveshapes](#waveshapes) section to learn more about those.

### Patch two

Ready for more? Try out this patch:

![Illustration of the second patch](images/8%20-%20Patch%202.svg){.dark-invert}

-   Turn Castor's pitch knob and pulse knob to 12 o' clock
-   Turn at least one of Castor's mix trimpots clockwise
-   Patch some V/oct CV from a sequencer, controller, or other source into Castor's pitch CV input jack.
-   Patch Castor's output jack to your rack's output or headphones module

Castor should now be playing notes based on the CV you're sending it. You can turn Castor's big pitch knob to tune/detune relative to the input. This is called _fine_ behavior and it's also explained in more detail in the [pitch](#pitch) section.

### Patch three

Okay, one last patch and we'll send you on your way:

![Illustration of the third patch](images/9%20-%20Patch%203.svg){.dark-invert}

-   Turn Castor's pitch knob and pulse knob to 12 o' clock
-   Turn Pollux's pitch knob and pulse knob to 12 o' clock
-   Turn at least one of Castor's mix trimpots clockwise
-   Turn at least one of Pollux's mix trimpots clockwise
-   Turn the LFO knob fully counter-clockwise
-   Turn the crossfade knob to 12 o' clock
-   Patch some V/oct CV from a sequencer, controller, or other source into Castor's pitch CV input jack.
-   Patch the crossfade output jack to your rack's output or headphones module

This is where things get fun- at this point, Castor & Pollux should both be playing (roughly) the same pitch and they're both coming through the crossfade output jack. Now, slowly wiggle Pollux's pitch knob. This is the most basic form of [oscillator stacking](#oscillator-stacking) and it's your gateway to the big, raw, and fun sounds of Castor & Pollux.

## Pitch behavior

Due to the intertwined nature of Castor & Pollux's oscillators, the effect of the pitch CV jack and knobs take on different _behaviors_ in different circumstances.

!!! fairy "Customization"
    You can modify Castor & Pollux's pitch behavior using the [settings editor](#editing-module-settings).

### Coarse

![Illustration of coarse pitch behavior](images/8%20-%20Coarse.svg){.dark-invert}

When [nothing](#jack-detection) is patched into Castor's pitch CV jack, `Coarse` behavior is used. Castor's pitch is determined by its pitch knob which sweeps through six octaves and quantizes to the nearest semitone.

### Fine

![Illustration of fine pitch behavior](images/9%20-%20Fine.svg){.dark-invert}

If there is a signal patched into Castor's pitch CV jack, `Fine` behavior is used. The input CV should be between `0 V` and `6 V`. The pitch knob offsets the input CV by `±1 octave`. The pitch knob has a sort of ["virtual notch"](#tuning) at the 12 o' clock position to help you dial in the frequency you're looking for.

Pollux also uses `Fine` behavior but will _follow_ Castor if nothing is patched into its pitch CV jack. Pollux doesn't ever use `Coarse` behavior.

### Multiply

![Illustration of multiply pitch behavior](images/10%20-%20multiply.svg){.dark-invert}

Finally, Pollux uses `Multiply` behavior when in [Hard Sync](#hard-sync) mode. In this case, Pollux follows Castor and the knob adds up to three octaves.

### Jack detection

Castor & Pollux detects whether a signal is patched into the pitch jacks by checking if the signal is above 0 Volts. In many cases this works well, however, you may run into issues if you use a sequencer or controller that often sends 0 Volt signals. Castor & Pollux will think that a jack isn't detected and erroneously switch to a different [pitch behavior](#pitch-behavior). If this is something you're running into you can disable jack detection using the [settings editor](#editing-module-settings).

## Tuning

Castor & Pollux is an analog oscillator with a digital brain. It's calibrated during assembly to accurately track 1 Volt / octave on the _pitch CV jacks_ and you can use the _pitch knobs_ to adjust the tuning of each oscillator.

In order to make tuning easier, Castor & Pollux provides two ways of using the pitch knob to dial in the perfect frequency.

<div class="card interactive">
    <div class="output">
        <label for="linear_tune">Linear response</label>
        <canvas id="linear_tune_canvas" width="640" height="50" class=""></canvas>
        <output id="linear_tune" name="linear_tune">440 Hz</output>
        <label for="non_linear_tune">Non-linear response</label>
        <canvas id="nonlinear_tune_canvas" width="640" height="50" class=""></canvas>
        <output id="non_linear_tune" name="non_linear_tune" class="teal">440 Hz</output>
    </div>
    <form id="tuning-form">
        <div class="slider">
            <input type="range" id="tuning_slider" name="tuning_slider"
                    min="-1.0" max="1.0" step="0.001" value="0.0">
            <label for="tuning_slider">pitch knob</label>
        </div>
    </form>
</div>

First, the pitch knobs have a "virtual notch" because of their **non-linear** response: they are less sensitive in the middle of their range than the edges. It's usually easier to understand this visually, so try out the little illustration above and notice that with the non-linear response it's much easier to tune to frequencies around `440 Hz`. This non-linear response only happens when the oscillator is using the `Follow` [pitch behavior](#pitch-behavior). You can configure how strong this effect is using the [settings editor](#editing-module-settings).

![Illustration of tweak mode pitch tuning](images/11%20-%20extra%20fine.svg){.dark-invert}

Second, the [tweak](#modes--tweaking) overlay allows you even more finely adjust tuning. Holding down the button and turning the pitch knob allows you to apply an additional `±2.5 semitone` offset. This offset is applied to the oscillator regardless of the [pitch behavior](#pitch-behavior) or the current [mode](#modes--tweaking).

## Waveshapes

Just like the Juno, Castor & Pollux generates three waveshapes: ramp, pulse, and sub.

The ramp wave looks like this (click the image to start and stop the animation):

<div class="interactive">
    <canvas id="ramp" width="640" height="480" class="waveform teal"></canvas>
</div>

!!! fairy "Simulation vs reality"
    These are _simulations_ of perfect waveshapes. Castor & Pollux's waveshapes are generated by analog circuitry and they don't quite look _exactly_ like these examples. That's a good thing - the analog weirdness is what adds character to the module.

The ramp sounds like this:

<div class="winter-audio-player">
    <audio title="ramp wave" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/ramp_octaves.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="ramp with filter" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/ramp_filtered.mp3"></audio>
</div>

The pulse wave depends on the pulse width CV and knob. You can vary the pulse width in this animation to see how it affects the waveshape:

<div class="interactive">
    <canvas id="pulse" width="640" height="480" class="waveform purple"></canvas>
    <form id="pulse-form">
        <div class="slider">
            <input type="range" id="pulse-width" name="width"
                    min="0" max="1.0" step="0.01" value="0.5" data-bind data-bind-type="float">
            <label for="pulse-width">pulse width</label>
        </div>
    </form>
</div>

You can also use [internal low-frequency oscillator](#internal-low-frequency-oscillator) to modulate the pulse width in [LFO PWM mode](#lfo-pwm).

Here are some examples of the pulse wave's sounds:

<div class="winter-audio-player">
    <audio title="pulse wave" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/square_octaves.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="pulse with pulse-width modulation" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/square_pwm.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="pulse with filter" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/square_filtered.mp3"></audio>
</div>

Finally, there's the sub waveshape. It's a square wave that's one octave lower. Here's what it looks like:

<div class="interactive">
    <canvas id="sub" width="640" height="480" class="waveform red"></canvas>
</div>

And here are some sound samples of the sub waveshape:

<div class="winter-audio-player">
    <audio title="sub wave" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/sub_octaves.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="sub with filter" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/sub_filtered.mp3"></audio>
</div>

These waveshapes can be mixed together to produce much more complex and interesting variants - try playing with the sliders under this animation to see how it affects the waveshape:

<div class="interactive">
    <canvas id="mix" width="640" height="480" class="waveform teal2"></canvas>
    <form id="mix-form">
        <div class="slider">
            <input type="range" id="ramp_mix" name="ramp_mix"
                    min="0" max="0.5" step="0.01" value="0.5" data-bind data-bind-type="float">
            <label for="ramp_mix">ramp volume</label>
        </div>
        <div class="slider">
            <input type="range" id="pulse_mix" name="pulse_mix"
                    min="0" max="0.5" step="0.01" value="0.25" data-bind data-bind-type="float">
            <label for="pulse_mix">pulse volume</label>
        </div>
        <div class="slider">
            <input type="range" id="pulse_width_mix" name="pulse_width"
                    min="0" max="1.0" step="0.01" value="0.5" data-bind data-bind-type="float">
            <label for="pulse_width_mix">pulse width</label>
        </div>
        <div class="slider">
            <input type="range" id="sub_mix" name="sub_mix"
                    min="0" max="0.5" step="0.01" value="0.25" data-bind data-bind-type="float">
            <label for="sub_mix">sub volume</label>
        </div>
    </form>
</div>

Here are some sound samples of various mixes:

<div class="winter-audio-player">
    <audio title="ramp & sub" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/ramp_and_sub.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="sub & pulse" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/ramp_and_square_with_pwm.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="various mixes" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/waveform_mixing.mp3"></audio>
</div>

## Oscillator stacking

Because Castor & Pollux has two oscillators, you can combine them together to build much more complex sounds. When there is no input into the second oscillator's pitch CV jack, it will follow the pitch of the the first oscillator. You can use the pitch knob of the second oscillator to _detune_ the second oscillator and cause interesting interactions with the first oscillator. You'll take your output from the **combined output** (marked by the symbol `♊︎`) and you can control the relative volume of the two oscillators using the **crossfader** (marked by the symbol `Σ`).

You can play around with this interactive animation to see how detuning and mixing changes the final waveshape:

<div class="interactive">
    <canvas id="stacked" width="640" height="480" class="waveform purple2"></canvas>
    <form id="stacked-form">
        <div class="slider">
            <input type="range" id="stacked_detune" name="detune"
                    min="-1" max="1" step="0.01" value="0.25" data-bind data-bind-type="float">
            <label for="stacked_detune">detuning</label>
        </div>
        <div class="slider">
            <input type="range" id="stacked_mix" name="mix"
                    min="0" max="1.0" step="0.01" value="0.5" data-bind data-bind-type="float">
            <label for="stacked_mix">crossfader (Σ)</label>
        </div>
    </form>
</div>

This animation just uses the ramp waveshape, but the crossfader takes the mix from each oscillator's mixer, so you can combine many different waveshapes. Here are some sound samples of oscillator stacking:

<div class="winter-audio-player">
    <audio title="stacked complex waveforms" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/stacked_complex.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="stacked sub & ramp" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/twilight.mp3"></audio>
</div>

<div class="winter-audio-player">
    <audio title="stacked sub & pulse" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/steps.mp3"></audio>
</div>

## Internal low-frequency oscillator

Castor & Pollux's built-in low-frequency oscillator is used throughout for modulating various parameters depending on the current [mode](#modes--tweaking).

![Illustration of LFO waveshape](images/20%20-%20lfo%20waveshape.svg)

By default, the internal LFO is a straightforward triangle wave. However, the [settings editor](#editing-module-settings) allows you to change the LFO's waveshape between triangle, sine, sawtooth, and square, as well as combine a _second_ waveshape with the first to create interesting LFO effects.

## Modes & tweaking

Castor & Pollux has four different _modes_ that change the overall module's functionality:

-   [Chorus](#chorus-mode) mode, the default mode, uses the internal LFO to modulate Pollux's frequency.
-   [LFO PWM](#lfo-pwm-mode) mode uses the internal LFO to modulate both oscillator's pulse width.
-   [LFO FM](#lfo-fm-mode) mode uses the internal LFO to modulate both oscillator's frequency.
-   [Hard Sync](#hard-sync-mode) mode produces metallic sounds by syncing Pollux's ramp core to Castor's.

![Illustration of tapping the button](images/12%20-%20tap%20button.svg){.dark-invert}

To cycle between modes, **tap** the button in the middle and the module will play a short animation to show that it has switched modes.

![Illustration of holding the button](images/13%20-%20hold%20button.svg){.dark-invert}

On the other hand, **holding** the button will turn on the tweak overlay. This gives you access to additional parameters depending on the mode. When moving in and out of the tweak overlay, the knobs get "latched" so that they don't immediately cause changes - similar how many synthesizers work when loading patches. The parameter only starts changing once you've moved the knob. In all modes, the pitch knobs control the extra-fine [tuning](#tuning).

### Chorus

Castor & Pollux's default mode is the _Chorus_ mode. This mode is inspired by the original Juno's analog chorus circuit, however, instead of applying the chorus affect _after_ sound generation, Castor & Pollux's chorusing works by varying the frequency of the second oscillator using its [internal low-frequency oscillator](#internal-low-frequency-oscillator). This means you have to [use both oscillators](#oscillator-stacking) in order to hear this effect and it works best if Pollux is _following_ Castor's pitch.

![Illustration of chorus controls](images/14%20-%20lfo%20controls.svg){.dark-invert}

The LFO knob, `φ`, determines the intensity of chorusing from none when fully counter-clockwise to its maximum at fully clockwise. The crossfade mixer, `Σ`, also has an impact on the intensity of the chorus.

![Illustration of chorus tweak controls](images/15%20-%20lfo%20tweaks.svg){.dark-invert}

When holding the tweak button, the LFO knob, `φ`, controls the LFO's frequency.

You can play around with this interactive animation to see how the chorusing amount changes the final waveshape. You'll need to click on the animation to start it otherwise you won't really be able to see the chorusing effect.

<div class="interactive">
    <canvas id="chorusing_" width="640" height="480" class="waveform red2"></canvas>
    <form id="chorusing-form">
        <div class="slider">
            <input type="range" id="chorusing_amount" name="amount"
                    min="0" max="1" step="0.01" value="0.25" data-bind data-bind-type="float">
            <label for="chorusing_amount">chorusing amount (φ)</label>
        </div>
    </form>
</div>

Here are some sound samples of chorusing:

<div class="winter-audio-player">
    <audio title="chorusing" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/chorusing.mp3"></audio>
</div>

### LFO PWM

LFO PWM mode uses the internal LFO to modulate the _pulse width_ of each oscillator. This only affects the _pulse_ [waveshape](#waveshapes).

![Illustration of the LFO PWM controls](images/16%20-%20lfo%20pwm.svg){.dark-invert}

The LFO knob, `φ`, determines the _frequency_ of the internal LFO. Meanwhile, each oscillator's pulse width knob controls the depth of modulation from none when fully counter-clockwise to its maximum at fully clockwise. Any signal patched into the pulse width jack is summed with the knob.

![Illustration of the LFO PWM tweak controls](images/17%20-%20lfo%20pwm%20tweaks.svg){.dark-invert}

When holding the tweak button, each oscillator's pulse width knob controls the _center_ of the pulse width modulation.

### LFO FM

LFO FM mode uses the internal LFO to modulate the _pitch_ for each oscillator. This is similar to the [Chorus mode](#chorus-mode), except it applies to both oscillators instead of just Pollux.

![Illustration of LFO FM controls](images/18%20-%20lfo%20fm.svg){.dark-invert}

The LFO knob, `φ`, determines the _frequency_ of the internal LFO. Meanwhile, each oscillator's pulse width knob controls the depth of pitch modulation from none when fully counter-clockwise to its maximum at fully clockwise.

Unlike the LFO PWM mode, the pulse width jack has no impact on modulation.

![Illustration of LFO FM tweak controls](images/19%20-%20lfo%20pwm%20tweaks.svg){.dark-invert}

When holding the tweak button, each oscillator's pulse width knob controls the oscillator's pulse width.

### Hard sync

[_Hard sync_](https://en.wikipedia.org/wiki/Oscillator_sync) is a feature seen in several synthesizer voices that have two or more oscillators. However, it's not a feature seen on the original Juno because each voice only had one oscillator. Luckily, Castor & Pollux has two oscillators!

![Illustration of hard sync where the first waveform is used to reset the second waveform](images/21%20-%20hard%20sync.svg)

_Hard sync_ mode syncs Pollux's oscillator to Castor's frequency - the output will retain the same _base frequency_ as Castor but the waveshape will be impacted by the Pollux's pitch. In other words, it'll sound like the same note but have a much different, usually metallic, timbre.

Since hard sync only affects Pollux, you'll have to use either Pollux's output or the crossfade output in order to hear the change in timbre.

Here are some sound samples of hard sync:

<div class="winter-audio-player">
    <audio title="hard sync" controls crossorigin="anonymous" src="https://storage.googleapis.com/files.winterbloom.com/gemini/hard_sync.mp3"></audio>
</div>

Hard sync mode's controls are the same as [chorus mode](#chorus-mode) except that Pollux's pitch knob uses the [Multiply behavior](#pitch-behavior):

!!! fairy "Hey, listen!"
    Due to the way hard sync works, you shouldn't set the second oscillator to a _lower_ frequency than the first. It'll just result in the sound growing softer since the second oscillator doesn't have enough time to output a complete wave cycle.

## Expander

![Illustration of C&P & expader next to each other](images/22%20-%20expander.svg){.dark-invert}

Castor & Pollux II includes a small expander that provides individual output jacks for each oscillator's [waveshapes](#waveshapes).

![Illustration of connecting the expander to C&P](images/Expander%20connection.svg){.dark-invert}

To use the expander, connect the small ribbon cable to the back of Castor & Pollux in the header labeled `Expander`. Connect the other end to the matching header on the back of the expander. Secure the expander to your case using screws.

!!! fairy "Lost cable?"
    Happens to the best of us. [Contact us](mailto:support@winterbloom.com) to get a replacement or grab a 10-pin 2x5 Socket-Socket 1.27mm IDC cable (also called a SWD cable) like these from [Adafruit](https://www.adafruit.com/product/1675) and [Sparkfun](https://www.sparkfun.com/products/15364).

## Connecting with USB

You can connect Castor & Pollux to your computer using a standard micro USB cable, which lets you [edit settings](#editing-module-settings) and [update the firmware](#updating-the-firmware).

![Illustration of connecting Castor & Pollux to USB](images/USB%20connection.svg){.dark-invert}

The micro USB port is located on the backside of the module. Once you've connected a cable, be careful not to put too much stress on the connector as it's possible to damage the connector with enough force.

!!! fairy "Power"
    Castor & Pollux II is happy to run on USB power when editing settings and updating firmware, but Castor & Pollux I needs to be powered from your rack power supply while connected to USB.

## Editing module settings

Even though Castor & Pollux uses analog circuitry to make its beautiful sounds, there is a little microcontroller brain inside that's orchestrating everything. There are lots of settings you can tweak over USB, such as the LED brightness, the range of the pitch knobs, quantization, and more.

Connect your module to your computer using [a USB cable](#connecting-with-usb) and then head over to the [web-based settings editor](settings) to tweak to your heart's content. You don't need to install any software or drivers.

## Updating the firmware

1. Download the latest firmware [from GitHub](https://github.com/wntrblm/Castor_and_Pollux/releases/latest). It will be a `.uf2` file.
1. Connect your module to your computer using [a USB cable](#connecting-with-usb).
1. Place Castor & Pollux in bootloader mode by quickly pressing the `Reset` button twice. The reset button is the small button next to the USB port. Once in bootloader mode, you should see an external drive on your computer named `GEMINIBOOT`.
1. Copy the `.uf2` file from step 1 to the `GEMINIBOOT` drive. The module should restart by itself.

## Open source hardware & software

Castor & Pollux is completely open source and hacking is encouraged.

-   The [firmware](https://github.com/wntrblm/Castor_and_Pollux/tree/main/firmware) is available under the [MIT License](https://github.com/wntrblm/Castor_and_Pollux/tree/main/firmware/LICENSE). Note that the firmware uses some third-party libraries that are under different, but compatible terms. Read the full text of the license for more details.
-   The [hardware designs](https://github.com/wntrblm/Castor_and_Pollux/tree/main/hardware) are available under the permissive [CERN-OHL-P v2](https://cern-ohl.web.cern.ch/) license, and is designed using [KiCAD](https://kicad.org/), which is free and open-source. You can open the hardware files using KiCAD, or you can check out a PDF of the schematics for the [mainboard](https://github.com/wntrblm/Castor_and_Pollux/raw/main/hardware/mainboard/mainboard.pdf) and the [jackboard](https://github.com/wntrblm/Castor_and_Pollux/raw/main/hardware/jackboard/jackboard.pdf).

![Open Source Hardware Association mark](images/oshw.svg){class=oshw} Castor & Pollux is [certified open source hardware](https://certification.oshwa.org/us001060.html).

## Warranty, disclaimers, and limits

Modules purchased from Winterbloom have a warranty of six months from the date of purchase. This warranty covers any manufacturing defects in the module. It does not cover damage due to incorrect handling, storage, power, overvoltage events, or modifications.

Please [contact us](mailto:support@winterbloom.com) if you are experiencing issues with your module. Modules returned under warranty will either be refunded, replaced, or repaired at our discretion. You will be responsible for the cost of returning the module to Winterbloom.

Because Eurorack has a vast number of possible combinations of power supplies, modules, cases, and accessories, it is impossible for us to guarantee compatibility with every possible configuration. We make a reasonable effort to test modules with several power supplies, cases, and other modules.

We encourage tinkering, however, we will not refund, repair, or replace any products that have been modified.

We have stress tested Castor & Pollux under the following conditions. These are provided for reference only; we do not guarentee that that the module will function under these conditions.

-   Continuous run time: at least 48 hours
-   Overvoltage from power supply: at least `±18 volts`
-   Reverse voltage from power supply: at least `±18 volts`
-   Undervoltage from power supply: as low as `±10 volts`
-   Over and under voltage on inputs: at least `±8 volts`
-   Output connected to another output: at least `±8 volts`
-   Output shorted to ground: at least 60 minutes

## Acknowledgments and thanks

Castor & Pollux would not be possible without the help of the Adafruit, support from Microchip, and the volumes of literature documenting and analyzing the Juno's DCO design.

[discord]: https://discord.gg/UpfqghQ

<link rel="stylesheet" href="styles/waveforms.css">
<script type="module" src="scripts/waveforms.js"></script>
<link rel="stylesheet" href="/winterjs/audio-player.css">
<script type="module" src="/winterjs/audio-player.js"></script>
