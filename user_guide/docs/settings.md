To use this editor:

1. Connect your Castor & Pollux to your Eurorack power supply. The module **must** be powered by the Eurorack power supply.
1. Connect a USB cable from your computer to Castor & Pollux's USB port. It's located on the right side of the module on the bottom circuit board.
1. Click the [connect button](#connect), and when prompted, allow the page access to your MIDI devices.
1. Once connected the editor will appear and load the current settings from your device.
1. Once you're done editing, click the save button at the bottom of the editor.
1. **Re-start your module for the changes to take effect**. You can do this by turning it off and back or on by pressing the small reset button that's right beside the USB port.

!!! Note
    This editor uses [WebMIDI](https://www.midi.org/17-the-mma/99-web-midi) to communicate with Castor & Pollux. It works in [Google Chrome](https://www.google.com/chrome/), [Microsoft Edge](https://www.microsoft.com/en-us/edge), and [Opera](https://www.opera.com/) ([full list of supported browsers](https://caniuse.com/midi)).

If you run into issues, feel free to [reach out](mailto:support@winterbloom.com).

<div class="field">
    <button class="button is-primary is-large is-fullwidth" type="button" id="connect">Connect</button>
    <p id="connect_info" class="notification hidden"></p>
</div>

<form class="form" id="settings_editor">
    <!-- Device information -->
    <div id="info_section" class="hidden">
    <div class="section-divider">Device info</div>
    <div id="firmware_incompatible" class="mb-5 hidden">
        <p class="notification is-danger">
            <span class="fairy" title="Hey, Listen!">🧚‍♀️</span>
            You'll need to update your module's firmware to use this editor. Don't worry, it's really easy! <a href="https://github.com/wntrblm/Castor_and_Pollux/releases/latest" target="_blank">Get the newest firmware</a>
        </p>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field device-info">
                <label class="label" for="firmware_version">Firmware version</label>
                <div class="control">
                    <div class="field has-addons">
                        <div class="control is-expanded">
                            <input class="input" id="firmware_version" type="text" name="firmware_version" readonly value=""/>
                        </div>
                        <div class="control">
                            <button type="button" class="button copy-button" title="copy to clipboard" data-clipboard-copy-target="firmware_version"><span class="material-icons">content_copy</span></button>
                        </div>
                    </div>
                </div>
            </div>
            <div class="field device-info">
                <label class="label" for="serial_number">Serial number</label>
                <div class="control">
                    <div class="field has-addons">
                        <div class="control is-expanded">
                            <input class="input device-info" id="serial_number" type="text" name="serial_number" readonly value="" />
                        </div>
                        <div class="control">
                            <button type="button" class="button copy-button" title="copy to clipboard" data-clipboard-copy-target="serial_number"><span class="material-icons">content_copy</span></button>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                This is your Castor & Pollux's current firmware version and serial number. We might ask you for this information if you reach out for help.
            </p>
            <p id="firmware_outdated" class="notification is-danger hidden">
                <span class="fairy" title="Hey, Listen!">🧚‍♀️</span>
                A new firmware version is available: <a href="https://github.com/wntrblm/Castor_and_Pollux/releases/latest" target="_blank"></a>
            </p>
        </div>
    </div>
    </div>
    <!-- Settings controls -->
    <div id="settings_section" class="hidden">
    <div class="section-divider">Settings</div>
    <div class="columns">
        <div class="column field">
            <label class="label" for="led_brightness">LED brightness</label>
            <input class="input" type="range" name="led_brightness" min="0" max="254" value="127" data-bind data-bind-type="int" />
            <span class="units"><span data-display-value-for="led_brightness" data-display-formatter="(input.valueAsNumber / 254 * 100).toFixed(0)"></span> percent</span>
        </div>
        <div class="column">
            <p class="notification help-text">The higher the brightness, the more power the module will consume. At maximum brightness, it consumes <code>130mA</code> from the <code>+12v</code> rail.</p>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label" for="base_cv_offset">Base pitch CV offset</label>
                <input class="input" type="number" name="base_cv_offset" value="1.0" step="0.083" min="0" max="5" data-bind data-bind-type="float" data-binding-precision="2" />
                <span class="units">octaves (volts)</span>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">This setting effectively determines the starting note for the module since the pitch CV input and offset knobs are added to this value to determine the final pitch CV. By default, it's <code>1.0</code>.</p>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label">Pitch quantizer</label>
            </div>
            <div class="field">
                <input class="checkbox" type="checkbox" id="castor_quantize" name="castor_quantize" value="off" data-bind />
                Quantize Castor pitch CV
            </div>
            <div class="field">
                <input class="checkbox" type="checkbox" id="pollux_quantize" name="pollux_quantize" value="off" data-bind />
                Quantize Pollux pitch CV
            </div>
        </div>
        <div class="column">
            <div class="notification help-text">
                <p>These settings determine whether to quantize the pitch CV inputs. This only affects the CV input, not the pitch knob, so that the oscillator can still be tuned. The base CV offset is added before quantization, so that the quantizer can be calibrated against an external CV source.</p>
                <p>If you find that Castor &amp; Pollux's pitch keeps jumping back and forth between adjacent notes, this means that the CV source is outputing values near to the boundary between two notes. Try increasing the base CV offset by 1/24 (ie, 0.0416...), to recenter the voltages properly.</p>
            </div>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label" for="castor_knob_min">Castor pitch knob minimum</label>
                <input class="input" type="number" name="castor_knob_min" value="-1.02" step="0.1" min="-3.3" max="0" data-bind data-bind-type="float" />
                <span class="units">octaves (volts)</span>
            </div>
            <div class="field">
                <label class="label" for="castor_knob_max">Castor pitch knob maximum</label>
                <input class="input" type="number" name="castor_knob_max" value="1.10" step="0.1" min="0" max="3.3" data-bind data-bind-type="float" />
                <span class="units">octaves (volts)</span>
            </div>
            <div class="field">
                <label class="label" for="pollux_knob_min">Pollux pitch knob minimum</label>
                <input class="input" type="number" name="pollux_knob_min" value="-1.02" step="0.1" min="-3.3" max="0" data-bind data-bind-type="float" />
                <span class="units">octaves (volts)</span>
            </div>
            <div class="field">
                <label class="label" for="pollux_knob_max">Pollux pitch knob maximum</label>
                <input class="input" type="number" name="pollux_knob_max" value="1.10" step="0.1" min="0" max="3.0" data-bind data-bind-type="float" />
                <span class="units">octaves (volts)</span>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">These settings determine the pitch offset knob's range for each oscillator. By default, they have a range of about <code>-1</code> to <code>+1</code> octaves (volts). The value of the pitch offset knob is added to the pitch CV input. Note that because the module's analog-to-digital converter (ADC) isn't perfect, the knobs don't quite reach 100% of their range. You may need to nudge these to give the knob some headroom.</p>
        </div>
    </div>
    <div class="columns">
        <div class="column field">
            <label class="label" for="pitch_knob_nonlinearity">Pitch knob non-linearity</label>
            <input class="input" type="range" name="pitch_knob_nonlinearity" value="0.6" step="0.01" min="0.33" max="1.0" data-bind data-bind-type="float" />
            <span class="units"><span data-display-value-for="pitch_knob_nonlinearity" data-display-format="percent"></span> percent</span>
        </div>
        <div class="column">
            <p class="notification help-text">
                The firmware adjusts the pitch CV knobs so that it's easier to tune
                the oscillators. It does this by spreading the values near the center
                of the knob out so that the range at the center is more fine. This
                makes the knobs non-linear. This setting controls how "intense"
                that non-linearity is. <code>0.33%</code> is equivalent to a linear
                response. Higher values make it easier and easier to tune the
                oscillator but sacrifices the range, values lower than <code>33%</code> willmake it harder to tune and aren't recommended. This is <code>60%</code> by default.
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column field">
            <label class="label" for="chorus_max_intensity">Chorus maximum intensity</label>
            <input class="input" type="range" name="chorus_max_intensity" value="0.05" step="0.01" min="0" max="1.0" data-bind data-bind-type="float" />
            <span class="units"><span data-display-value-for="chorus_max_intensity" data-display-format="percent"></span> percent</span>
        </div>
        <div class="column">
            <p class="notification help-text">
                This setting controls the <strong>maximum</strong> intensity of the chorusing feature. The intensity is the amount that the internal LFO effects the pitch of the second oscillator (Pollux).
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column field">
            <label class="label" for="lfo_1_frequency">LFO frequency</label>
            <input class="input" type="range" name="lfo_1_frequency" value="0.2" step="0.1" min="0.1" max="10.0" data-bind data-bind-type="float"/>
            <span class="units"><span data-display-value-for="lfo_1_frequency"></span> hertz</span>
        </div>
        <div class="column">
            <p class="notification help-text">
                This setting controls the <strong>default</strong> frequency of the internal low-frequency oscillator (LFO). This LFO is used by the chorusing feature and the pulse-width modulation (PWM) feature. The frequency can be changed anytime by holding the <em>hard sync</em> button and moving the <em>chorusing amount</em> (<code>φ</code>) knob, this just sets the frequency at start-up.
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column field">
            <label class="label">LFO waveform</label>
            <canvas id="lfo-waveform-canvas" width="640" height="200" class="waveform teal"></canvas>
            <div class="columns">
                <div class="column">
                    <div class="select is-full-width">
                        <select id="lfo_1_waveshape" name="lfo_1_waveshape" data-bind data-bind-type="int">
                            <option value="0">Triangle</option>
                            <option value="1">Sine</option>
                            <option value="2">Sawtooth</option>
                            <option value="3">Square</option>
                        </select>
                    </div>
                    <input class="input" type="range" id="lfo_1_factor" name="lfo_1_factor" value="1.0" step="0.01" min="0.0" max="1.0" data-bind data-bind-type="float"/>
                    <label class="label-nested" for="lfo_1_factor">amplitude</label>
                </div>
                <div class="column">
                    <div class="select is-full-width">
                        <select id="lfo_2_waveshape" name="lfo_2_waveshape" data-bind-type="int">
                            <option value="0">Triangle</option>
                            <option value="1">Sine</option>
                            <option value="2">Sawtooth</option>
                            <option value="3">Square</option>
                        </select>
                    </div>
                    <input class="input" type="range" id="lfo_2_factor" name="lfo_2_factor" value="1.0" step="0.01" min="0.0" max="1.0" data-bind data-bind-type="float"/>
                    <label class="label-nested" for="lfo_2_factor">amplitude</label>
                    <input class="input" type="range" id="lfo_2_frequency_ratio" name="lfo_2_frequency_ratio" value="1.0" step="0.01" min="0.0" max="10.0" data-bind data-bind-type="float"/>
                    <label class="label-nested" for="lfo_2_frequency_ratio">frequency ratio</label>
                </div>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                These settings control the overall waveshape of the internal LFO.
                The LFO is generated by adding two indepedent waveforms together.
                By default, the first waveform is a triangle wave and the second waveform is turned off.
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label">LFO routing</label>
            </div>
            <div class="field">
                <label class="checkbox" for="castor_lfo_pwm">
                    <input class="checkbox" type="checkbox" id="castor_lfo_pwm" name="castor_lfo_pwm" value="on" data-bind />
                    Route LFO to Castor's pulse width
                </label>
            </div>
            <div class="field">
                <label class="checkbox" for="pollux_lfo_pwm">
                    <input class="checkbox" type="checkbox" id="pollux_lfo_pwm" name="pollux_lfo_pwm" value="on" data-bind />
                    Route LFO to Pollux's pulse width
                </label>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                These settings control whether the internal LFO is used to modulate the pulse width of each oscillator by default. The routing can be enabled or disabled at any time by holding <em>hard sync</em> button and moving the oscillator's <em>pulse width</em> knob, this just enables or disables it at start-up.
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label" for="smooth_initial_gain">CV smoothing initial gain</label>
                <input class="input" type="range" name="smooth_initial_gain" value="0.1" step="0.05" min="0" max="1" data-bind data-bind-type="float" data-binding-precision="4" />
                <span class="units"><span data-display-value-for="smooth_initial_gain" data-display-format="percent"></span> percent</span>
            </div>
            <div class="field">
                <label class="label" for="smooth_sensitivity">CV smoothing sensitivity</label>
                <input class="input" type="range" name="smooth_sensitivity" value="20" step="1" min="0" max="100" data-bind data-bind-type="float" />
                <span class="units"><span data-display-value-for="smooth_sensitivity"></span>x multiplier</span>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                These settings control how the module filters the pitch CV inputs. Due to inherent limitations with converting analog pitch CV to a digital pitch value, such as noise and transient errors, it's necessary to basically <em>low-pass filter</em> (or <em>slew limit</em>) the readings. The module's filter is a little more sophisticated than that- it is a self-modulated low-pass filter. The <strong>initial gain setting</strong> determines the minimum effect of a new reading on the ongoing average - so if it is 0.1% then it will effect the ongoing average by 10%, if it is 1.0 then previous readings are essentially ignored because it effects the ongoing average by 100%. The <strong>sensitivity</strong> setting determines how much the low-pass filter can self-modulate. Higher settings means that the module will be more reactive to large changes in the CV input, whereas lower settings will cause the module to react very slowly to any change in the CV input.
            </p>
        </div>
    </div>
    <div class="section-divider">Danger zone</div>
    <div class="columns">
        <div class="column field">
            <p>These settings are calibrated during assembly and <strong>do not have default values</strong>. Changing these settings can cause your module to behave incorrectly. It's highly recommended that you write down these settings before making changes in case you need to restore them.</p>
            <label class="checkbox danger-zone" for="allow_danger">
                <input class="checkbox" type="checkbox" id="allow_danger" />
                Let me edit dangerous settings
            </label>
        </div>
    </div>
    <div class="columns">
        <div class="column field">
            <label class="label" for="pollux_follower_threshold">Pollux follower threshold</label>
            <input class="input is-dangerous" type="number" name="pollux_follower_threshold" min="0" max="500" value="100" readonly data-bind data-bind-type="int"/>
            <span class="units"><span data-display-value-for="pollux_follower_threshold"></span> code points, <span data-display-value-for="pollux_follower_threshold" data-display-formatter="((input.valueAsNumber / 4096) * 6.0).toFixed(2);"></span> volts</span>
        </div>
        <div class="column">
            <p class="notification help-text">
                These settings controls how Pollux determines when to follow Castor. The intended behavior is that when no cable is attached to Pollux's pitch CV jack, it will follow Castor's pitch. The module detects this condition by seeing if the input jack is reading 0 volts. Sometimes due to analog conversion errors this threshold needs to be adjusted. Lower thresholds may lead to glitchy behavior as the noise measures above and below the threshold, and higher settings may lead to Pollux ignoring the pitch CV input on lower notes.
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label" for="cv_gain_error">CV input gain correction</label>
                <input class="input is-dangerous" type="number" name="cv_gain_error" value="1" min="0.5" max="2.0" step="0.0001" readonly data-bind data-bind-type="float" data-binding-precision="4"/>
                <span class="units">multiplier</span>
            </div>
            <div class="field">
                <label class="label" for="cv_offset_error">CV input error correction</label>
                <input class="input is-dangerous" type="number" name="cv_offset_error" value="0" readonly data-bind-type="float" data-bind data-binding-precision="1"/>
                <span class="units">code points</span>
            </div>
            <div class="field">
                <label class="label" for="adc_gain_corr">ADC gain correction</label>
                <input class="input is-dangerous" type="number" name="adc_gain_corr" value="2048" min="1024" max="3072" readonly data-bind data-bind-type="int"/>
                <span class="units"><span data-display-value-for="adc_gain_corr" data-display-formatter="(input.valueAsNumber / 2048).toFixed(3)"></span>x multiplier</span>
            </div>
            <div class="field">
                <label class="label" for="adc_offset_corr">ADC offset correction</label>
                <input class="input is-dangerous" type="number" name="adc_offset_corr" value="0" min="-100" max="100" readonly data-bind data-bind-type="int" />
                <span class="units">code points</span>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                These settings determine how the module corrects for gain and offset errors in its analog-to-digital converter (ADC). We calibrate every Castor &amp; Pollux during manufacturing and save the calibration data to the device. However, we keep a copy of it in case it somehow gets erased or corrupted. You can use the restore button below to load the values that were originally on your device. If the button is greyed out, then we don't have a copy of your module's calibration data. ☹️
            </p>
            <div class="field">
                <button class="button is-warning" type="button" id="restore_adc_calibration">Restore to factory values</button>
            </div>
        </div>
    </div>
    <div class="section-divider">Save & restart</div>
    <div class="field">
        <button class="button is-primary is-large is-fullwidth" type="button" id="save_button">Save</button>
        <p class="has-text-centered">Remember to save and <strong>restart your module</strong> to have these settings take effect!</p>
    </div>
    </div>
</form>

<script type="module" src="../scripts/settings.js"></script>
<link rel="stylesheet" href="../styles/settings.css"/>
