To use this editor:

1. Connect your Castor & Pollux to your Eurorack power supply. The module **must** be powered by the Eurorack power supply.
1. Connect a USB cable from your computer to Castor & Pollux's USB port. It's located on the right side of the module on the bottom circuit board.
1. Click the [connect button](#connect), and when prompted, allow the page access to your MIDI devices.
1. Once connected the editor will appear and load the current settings from your device.
1. Once you're done editing, click the save button at the bottom of the editor.
1. Saving should automatically reset your module, but, if you don't see the changes take effect then try restarting your module. You can do this by turning it off and back on or by pressing the small reset button that's right beside the USB port.

!!! Note
    This editor uses [WebMIDI](https://www.midi.org/17-the-mma/99-web-midi) to communicate with Castor & Pollux. It works in [Google Chrome](https://www.google.com/chrome/), [Microsoft Edge](https://www.microsoft.com/en-us/edge), and [Opera](https://www.opera.com/) ([full list of supported browsers](https://caniuse.com/midi)).

If you run into issues, feel free to [reach out](mailto:support@winterbloom.com).

<div class="field">
    <button class="button is-primary is-large" type="button" id="connect">Connect</button>
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
                <label class="label" for="harware_revision">Hardware revision</label>
                <div class="control">
                    <div class="field">
                        <div class="control is-expanded">
                            <input class="input device-info" id="hardware_revision" type="text" name="hardware_revision" readonly value="" />
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
                This is your Castor & Pollux's current firmware version, hardware revision, and serial number. We might ask you for this information if you reach out for help. C&PI is hardware revisions 1 through 4 and C&PII is hardware revisions 5 and above.
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
                oscillator but sacrifices the range, values lower than <code>33%</code>
                will make it harder to tune and aren't recommended. This is
                <code>60%</code> by default.
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label">Quantization</label>
            </div>
            <div class="field">
                <label class="checkbox" for="quantization_enabled">
                    <input class="checkbox" type="checkbox" id="quantization_enabled" name="quantization_enabled" value="on" data-bind />
                    Enable Castor's semitone quantization
                </label>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                When nothing is patched into Castor's pitch CV input, Castor's pitch knob becomes a <em>coarse</em> pitch knob that sweeps across 6 octaves. By default, in this situation Castor will quantize to the nearest semitone. You can disable this to get continuous frequency changes.
            </p>
        </div>
    </div>
    <div class="columns">
        <div class="column">
            <div class="field">
                <label class="label">Jack detection</label>
            </div>
            <div class="field">
                <label class="checkbox" for="zero_detection_enabled">
                    <input class="checkbox" type="checkbox" id="zero_detection_enabled" name="zero_detection_enabled" value="on" data-bind />
                    Enable pitch jack detection
                </label>
            </div>
            <div class="field">
                <label class="label" for="zero_detection_threshold">Zero detection threshold</label>
                <input class="input" type="number" name="zero_detection_threshold" min="0" max="800" value="100" data-bind data-bind-type="int"/>
                <span class="units"><span data-display-value-for="zero_detection_threshold"></span> code points
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                These settings determine how C&P detects the presence or absence of a signal at the pitch CV jacks. C&P does this by checking if the signal at the jack is <code>0V</code>.<br/>
                <br/>
                The intended behavior is that when no cable is patched to Castor's pitch input, it'll switch to "coarse" pitch behavior. Likewise for Pollux, if there's no cable patch to its pitch CV then it will follow Castor. However, some users may find this behavior undesirable, especially if you often send pitch CV of around <code>0V</code>.
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
                        <select id="lfo_2_waveshape" name="lfo_2_waveshape" data-bind data-bind-type="int">
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
                <label class="label">Pulse width behavior</label>
                <div class="select is-full-width">
                    <select id="pulse_width_bitmask" name="pulse_width_bitmask" data-bind data-bind-type="int">
                        <option value="4095">Smooth</option>
                        <option value="3840">Classic</option>
                    </select>
                </div>
            </div>
        </div>
        <div class="column">
            <p class="notification help-text">
                This controls the behavior of the pulse wave when the pulse width is modulated. Smooth is almost certainly what you want, however, if you choose Classic you'll introduce some interesting glitchy/stepping behavior when the pulse width changes.
                <br>
                The reason this setting exists is because prior to the July 2022 firmware release, Castor &amp; Pollux had a bug that caused the effective pulse width resolution to be far lower than intended. Some folks like the old sound, so we added this setting for nostalgia.
            </p>
        </div>
    </div>
    <div class="section-divider">Danger zone</div>
    <div class="columns">
        <div class="column field">
            <p>These settings are calibrated during assembly and <strong>do not have default values</strong>. Changing these settings can cause your module to behave incorrectly, so do not touch them unless you know what you're doing or have been asked to do so by support. It's highly recommended that you write down these settings before making changes in case you need to restore them.</p>
            <label class="checkbox danger-zone" for="allow_danger">
                <input class="checkbox" type="checkbox" id="allow_danger" />
                Let me edit dangerous settings
            </label>
        </div>
    </div>
    <div class="columns">
        <div class="column field">
            <label class="label" for="osc8m_freq">8 MHz clock measured frequency</label>
            <input class="input is-dangerous" type="number" name="osc8m_freq" min="7000000" max="9000000" value="8000000" readonly data-bind data-bind-type="int"/>
            <span class="units">Hertz</span>
        </div>
        <div class="column">
            <p class="notification help-text">
                Castor & Pollux uses an internal 8 MHz clock to generate the frequency signals for the oscillators. The actual frequency of this oscillator can vary between 7.94 MHz and 8.06 MHz. This setting is set to compensate for that so that the oscillator outputs are in tune.
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
    <div class="columns">
        <div class="column field">
            <label class="label">Tuning calibration</label>
            <div class="columns">
                <div class="column is-narrow">
                    <button type="button" class="button is-info" id="measure_one">Measure 1V</button>
                </div>
                <div class="column">
                    <input id="measure_one_result" class="input" type="number" placeholder="not measured" readonly>
                </div>
            </div>
            <div class="columns">
                <div class="column is-narrow">
                    <button type="button" class="button is-info" id="measure_three">Measure 3V</button>
                </div>
                <div class="column">
                    <input id="measure_three_result" class="input" type="number" placeholder="not measured" readonly>
                </div>
            </div>
            <div class="columns">
                <div class="column">
                    <label class="label" for="gain_error_result">Gain error</label>
                    <input id="gain_error_result" name="gain_error_result" class="input" type="number" readonly/>
                </div>
                <div class="column">
                    <label class="label" for="offset_error_result">Offset error</label>
                    <input id="offset_error_result" name="offset_error_result" class="input" type="number" readonly/>
                </div>
            </div>
            <button type="button" class="button is-danger is-fullwidth" id="save_tuning" disabled>Save</button>
        </div>
        <div class="column">
            <p class="notification help-text">
                This allows you to re-calibrate your module's tuning. You should only do this if you absolutely know what you're doing or have been instructed to do so by support. <strong>Please write down the values for the settings in the previous section</strong> as this will overwrite them.
                <br>
                To use this, connect the most accurate CV source that you have to the first oscillator's pitch input. Set the CV to 1V and click "Measure 1V". Repeat for 3V. Once both measurements have been taken, it will calculate new values for tuning calibration. Click "Save" to write these to the device.
            </p>
        </div>
    </div>
    <div id="monitor_section" class="hidden">
        <div class="section-divider"></div>
        <div class="field">
            <label class="label">Monitor</label>
            <button class="button is-warning" type="button" id="enable_monitor">Enable monitoring</button>
            <pre class="code" id="monitor_output">monitor output</pre>
        </div>
    </div>
    <div class="section-divider"></div>
    <div class="field">
        <button class="button is-primary is-large is-fullwidth" type="button" id="save_button">Save &amp; restart</button>
    </div>
    </div>
</form>

<script type="module" src="../scripts/settings.js"></script>
<link rel="stylesheet" href="../styles/settings.css"/>
