!!! warning "Nothing to see here, move along"
    Whoa there, no one has a Castor & Pollux yet, so you shouldn't be here! Don't worry - it'll be ready to use before we start shipping units!

This page uses [WebMIDI](https://www.midi.org/17-the-mma/99-web-midi) to communicate with Castor & Pollux and modify internal settings. This works in [Google Chrome](https://www.google.com/chrome/), [Microsoft Edge](https://www.microsoft.com/en-us/edge), and [Opera](https://www.opera.com/) ([full list of supported browsers](https://caniuse.com/midi)).

To use this editor:

1. Connect your Castor & Pollux to your Eurorack power supply. The module **must** be powered by the Eurorack power supply.
1. Connect a USB cable from your computer to Castor & Pollux's USB port. It's located on the right side of the module on the bottom circuit board.
1. Click the connect button, and when prompted, allow the page access to your MIDI devices.
1. Once connected the editor will appear and load the current settings from your device.
1. Once you're done editing, click the save button at the bottom of the editor. You'll need to restart your module for the changes to take effect.

If you run into issues, feel free to [reach out](mailto:support@winterbloom.com).

<button id="connect" class="btn btn-primary btn-lg">Connect</button><span id="connect_info" class="connect-info"></span>

<form id="settings_editor" class="settings-editor hidden">
    <fieldset>
        <legend>Settings editor</legend>
        <div class="form-group">
            <label for="led_brightness">LED brightness</label>
            <input type="range" name="led_brightness" class="form-control" min="0" max="127" value="127" />
        </div>
        <span class="form-message">The following settings set the range of the pitch knobs for each oscillator. By default, they have a range of -1.01 to 1.01 octaves (volts). The value of the pitch knob is added to the pitch CV input</span>
        <div class="form-group">
            <label for="castor_knob_min">Castor pitch knob minimum</label>
            <input type="number" name="castor_knob_min" class="form-control" value="-1.01" step="0.1" min="-3.0" max="0" />
            <span class="form-unit">octaves (volts)</span>
        </div>
        <div class="form-group">
            <label for="castor_knob_max">Castor pitch knob maximum</label>
            <input type="number" name="castor_knob_max" class="form-control" value="1.01" step="0.1" min="0" max="3.0" />
            <span class="form-unit">octaves (volts)</span>
        </div>
        <div class="form-group">
            <label for="pollux_knob_min">Pollux pitch knob minimum</label>
            <input type="number" name="pollux_knob_min" class="form-control" value="-1.01" step="0.1" min="-3.0" max="0" />
            <span class="form-unit">octaves (volts)</span>
        </div>
        <div class="form-group">
            <label for="pollux_knob_max">Pollux pitch knob maximum</label>
            <input type="number" name="pollux_knob_max" class="form-control" value="1.01" step="0.1" min="0" max="3.0" />
            <span class="form-unit">octaves (volts)</span>
        </div>
        <span class="form-message">The following settings control the intensity and frequency of the chorusing feature. The intensity of the chorus is determined by the chorus knob and its range is set by the <strong>maximum intensity</strong> setting. The chorus operates at a fixed frequency determined by the <strong>frequency</strong> setting.</span>
        <div class="form-group">
            <label for="chorus_max_intensity">Chorus maximum intensity</label>
            <input type="range" name="chorus_max_intensity" class="form-control" value="0.05" step="0.01" min="0" max="1.0" />
            <span class="form-unit"><span id="chorus_max_intensity_display_value"></span> percent</span>
        </div>
        <div class="form-group">
            <label for="chorus_frequency">Chorus frequency</label>
            <input type="range" name="chorus_frequency" class="form-control" value="0.2" step="0.1" min="0.1" max="5.0" />
            <span class="form-unit"><span id="chorus_frequency_display_value"></span> hertz</span>
        </div>
        <span class="form-message">The following settings control how the module filters the pitch CV inputs. Due to inherent limitations with converting analog pitch CV to a digital pitch value, such as noise and transient errors, it's necessary to basically <em>low-pass filter</em> (or <em>slew limit</em>) the readings. The module's filter is a little more sophisticated than that- it is a self-modulated low-pass filter. The <strong>initial gain setting</strong> settings the minimum affect of a new reading on the ongoing average - so if it is 0.1% then it will effect the ongoing average by 10%, if it is 1.0 then previous readings are essentially ignored because it effects the ongoing average by 100%. The <strong>sensitivity</strong> setting determines how much the low-pass filter can self-modulate. Higher settings means that the module will be more reactive to large changes in the CV input, whereas lower settings will cause the module to react very slowly to any change in the CV input.</span>
        <div class="form-group">
            <label for="smooth_initial_gain">CV smoothing initial gain</label>
            <input type="range" name="smooth_initial_gain" class="form-control" value="0.1" step="0.05" min="0" max="1" />
            <span class="form-unit"><span id="smooth_initial_gain_display_value"></span> percent</span>
        </div>
        <div class="form-group">
            <label for="smooth_sensitivity">CV smoothing sensitivity</label>
            <input type="range" name="smooth_sensitivity" class="form-control" value="20" step="1" min="0" max="100" />
            <span class="form-unit"><span id="smooth_sensitivity_display_value"></span>x multiplier</span>
        </div>
        <!-- Scary settings -->
        <legend>Danger zone</legend>
        <p>These settings are calibrated during assembly and <strong>do not have default values</strong>. Changing these settings can cause your module to behave incorrectly. It's highly recommended that you write down these settings before making changes in case you need to restore them.</p>
        <div class="form-group danger-zone">
            <label for="allow_danger">
            <input type="checkbox" id="allow_danger" /> Let me edit dangerous settings</label>
        </div>
        <span class="form-message">The following setting controls how Pollux determines when to follow Castor. The intended behavior is that when no cable is attached to Pollux's pitch CV jack, it will follow Castor's pitch. The module detects this condition by seeing if the input jack is reading 0 volts. Sometimes due to ADC errors this threshold needs to be adjusted. Lower thresholds may lead to glitchy behavior as the ADC noise measures above and below the threshold, and higher settings may lead to Pollux ignoring the pitch CV input on lower notes.</span>
        <div class="form-group">
            <label for="pollux_follower_threshold">Pollux follower threshold</label>
            <input type="range" name="pollux_follower_threshold" class="form-control" value="6" step="1" min="0" max="20" disabled />
            <span class="form-unit"><span id="pollux_follower_threshold_display_value"></span> code points</span>
        </div>
        <span class="form-message">The following settings determine how the module corrects for gain and offset errors in its ADC.</span>
        <div class="form-group">
            <label for="adc_gain_corr">Knob gain correction</label>
            <input type="number" name="knob_gain_corr" class="form-control" value="1" min="0.5" max="2.0" readonly />
            <span class="form-unit"><span id="knob_gain_corr_display_value"></span>x multiplier</span>
        </div>
        <div class="form-group">
            <label for="knob_gain_corr">Knob offset correction</label>
            <input type="number" name="knob_offset_corr" class="form-control" value="0" readonly />
            <span class="form-unit"><span id="knob_offset_corr_display_value"></span> code points</span>
        </div>
        <div class="form-group">
            <label for="adc_gain_corr">ADC gain correction</label>
            <input type="number" name="adc_gain_corr" class="form-control" value="2048" min="1024" max="3072" readonly />
            <span class="form-unit"><span id="adc_gain_corr_display_value"></span>x multiplier</span>
        </div>
        <div class="form-group">
            <label for="adc_offset_corr">ADC offset correction</label>
            <input type="number" name="adc_offset_corr" class="form-control" value="0" min="-100" max="100" readonly />
            <span class="form-unit"><span id="adc_offset_corr_display_value"></span> code points</span>
        </div>
        <div class="form-group">
            <button type="button" id="save_button" class="btn btn-primary btn-lg btn-block">Save</button>
        </div>
    </fieldset>
</form>

<link rel="stylesheet" href="../styles/settings.css" />
<script type="module" src="../scripts/settings.js"></script>