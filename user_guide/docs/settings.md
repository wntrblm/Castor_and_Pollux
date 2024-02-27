To use this editor:

1. Connect your Castor & Pollux to your Eurorack power supply. The module **must** be powered by the Eurorack power supply.
1. Connect a USB cable from your computer to Castor & Pollux's USB port. It's located on the right side of the module on the bottom circuit board.
1. Click the ++"connect"++ button below, and when prompted, allow the page access to your MIDI devices.
1. Once connected the editor will appear and load the current settings from your device.
1. Once you're done editing, click the ++"save & restart"++ button at the bottom of the editor.
1. Saving should automatically reset your module, but, if you don't see the changes take effect then try restarting your module. You can do this by turning it off and back on or by pressing the small reset button that's right beside the USB port.

!!! Note
    This editor uses [WebMIDI](https://www.midi.org/17-the-mma/99-web-midi) to communicate with Castor & Pollux. It works in [Google Chrome](https://www.google.com/chrome/), [Microsoft Edge](https://www.microsoft.com/en-us/edge), and [Opera](https://www.opera.com/) ([full list of supported browsers](https://caniuse.com/midi)).

If you run into issues, feel free to [reach out](mailto:support@winterbloom.com).


<button class="button is-primary is-large" type="button" id="connect">Connect</button>

<div id="connect_info" class="admonition danger" hidden>
  <p class="admonition-title">Connection info</p>
</div>

<div id="firmware_incompatible" class="admonition fairy" hidden>
  <p class="admonition-title">Firmware update required</p>
  <p>You'll need to update your module's firmware to use this editor. Don't worry, it's really easy! <a href="https://github.com/wntrblm/Castor_and_Pollux/releases/latest" target="_blank">Get the newest firmware</a>
</div>

<form class="form" id="settings_editor">
  <!-- Device information -->
  <section id="info_section" hidden>
    <h2>Device info</h2>
    <div class="grid">
      <div>
        <label for="firmware_version">Firmware version</label>
        <div role="group">
          <input id="firmware_version" type="text" name="firmware_version" readonly value="" />
          <button type="button" title="copy to clipboard" data-clipboard-copy-target="firmware_version">
              <winter-icon>content_copy</winter-icon></button>
        </div>
        <label for="hardware_revision">Hardware revision</label>
        <input id="hardware_revision" type="text" name="hardware_revision" readonly value="" />
        <label for="serial_number">Serial number</label>
        <div role="group">
          <input id="serial_number" type="text" name="serial_number" readonly value="" />
          <button type="button" title="copy to clipboard" data-clipboard-copy-target="serial_number">
            <winter-icon>content_copy</winter-icon></button>
        </div>
      </div>
      <aside>
        This is your C&P's current firmware version, hardware revision, and serial number. We might ask you for this information if you reach out for help. C&PI is hardware revisions <code>1-4</code> and C&PII is hardware revisions <code>5</code> and above.
      </aside>
    </div>
  </section> <!-- info_section -->
  <!-- General settings -->
  <section id="settings_section" hidden>
    <h2>Settings</h2>
    <div class="grid col-2">
      <div>
        <label for="led_brightness">LED brightness</label>
        <input type="range" name="led_brightness" min="0" max="254" value="127" data-bind data-bind-type="int" />
        <small><span data-display-value-for="led_brightness" data-display-formatter="(input.valueAsNumber / 254 * 100).toFixed(0)"></span> percent</small>
      </div>
      <aside>
        The higher the brightness, the more power the module will consume. At maximum brightness, it consumes <code>130mA</code> from the <code>+12V</code> rail.</aside>
      <div>
        <label for="base_cv_offset">Base pitch CV offset</label>
        <input type="number" name="base_cv_offset" value="1.0" step="0.083" min="0" max="5" data-bind data-bind-type="float" data-binding-precision="2" />
        <small>octaves (volts)</small>
      </div>
      <aside>
        This setting effectively determines the starting note for the module since the pitch <abbr title="Control voltage">CV</abbr> input and offset knobs are added to this value to determine the final pitch CV. By default, it's <code>1.0</code>.
      </aside>
      <div>
        <label for="castor_knob_min">Castor pitch knob minimum</label>
        <input type="number" name="castor_knob_min" value="-1.02" step="0.1" min="-3.3" max="0" data-bind data-bind-type="float" />
        <small>octaves (volts)</small>
        <label for="castor_knob_max">Castor pitch knob maximum</label>
        <input type="number" name="castor_knob_max" value="1.10" step="0.1" min="0" max="3.3" data-bind data-bind-type="float" />
        <small>octaves (volts)</small>
        <label for="pollux_knob_min">Pollux pitch knob minimum</label>
        <input type="number" name="pollux_knob_min" value="-1.02" step="0.1" min="-3.3" max="0" data-bind data-bind-type="float" />
        <small>octaves (volts)</small>
        <label for="pollux_knob_max">Pollux pitch knob maximum</label>
        <input type="number" name="pollux_knob_max" value="1.10" step="0.1" min="0" max="3.0" data-bind data-bind-type="float" />
        <small>octaves (volts)</small>
      </div>
      <aside>
        These settings determine the pitch offset knob's range for each oscillator. By default, they have a range of about <code>-1</code> to <code>+1</code> octaves (volts). The value of the pitch offset knob is added to the pitch <abbr title="Control voltage">CV</abbr> input. Note that because the module's <abbr title="Analog to digital converter">ADC</abbr> isn't perfect, the knobs don't quite reach 100% of their range. You may need to nudge these to give the knob some headroom.
      </aside>
      <div>
        <label for="pitch_knob_nonlinearity">Pitch knob non-linearity</label>
        <input type="range" name="pitch_knob_nonlinearity" value="0.6" step="0.01" min="0.33" max="1.0" data-bind data-bind-type="float" />
        <small>
          <span data-display-value-for="pitch_knob_nonlinearity" data-display-format="percent"></span> percent</small>
      </div>
      <aside>
          The firmware adjusts the pitch <abbr title="Control voltage">CV</abbr> knobs so that it's easier to tune the oscillators. It does this by spreading the values near the center of the knob out so that the range at the center is more fine. This makes the knobs non-linear. This setting controls how "intense" that non-linearity is. <code>33%</code> is equivalent to a linear response. Higher values make it easier and easier to tune the oscillator but sacrifices the range, values lower than <code>33%</code> will make it harder to tune and aren't recommended. This is <code>60%</code> by default.
      </aside>
      <div>
        <label>Quantization</label>
        <label for="quantization_enabled">
          <input type="checkbox" id="quantization_enabled" name="quantization_enabled" value="on" data-bind />
          Enable Castor's semitone quantization
        </label>
      </div>
      <aside>
        When nothing is patched into Castor's pitch <abbr title="Control voltage">CV</abbr> input, Castor's pitch knob becomes a <em>coarse</em> pitch knob that sweeps across <code>6</code> octaves. By default, in this situation Castor will quantize to the nearest semitone. You can disable this to get continuous frequency changes.
      </aside>
      <div>
        <label>Jack detection</label>
        <label for="zero_detection_enabled">
          <input type="checkbox" id="zero_detection_enabled" name="zero_detection_enabled" value="on" data-bind />
          Enable pitch jack detection
        </label>
        <label for="zero_detection_threshold">Zero detection threshold</label>
        <input type="number" name="zero_detection_threshold" min="0" max="800" value="100" data-bind data-bind-type="int" />
        <small><span data-display-value-for="zero_detection_threshold"></span> code points
            </small>
      </div>
      <aside>
        These settings determine how C&P detects the presence or absence of a signal at the pitch <abbr title="Control voltage">CV</abbr> jacks. C&P does this by checking if the signal at the jack is <code>0V</code>.
        <br/><br/>
        The intended behavior is that when no cable is patched to Castor's pitch input, it'll switch to "coarse" pitch behavior. Likewise for Pollux, if there's no cable patched to its pitch input then it will follow Castor. However, some users may find this behavior undesirable, especially if you often send pitch CV of around <code>0V</code>.
      </aside>
      <div>
        <label for="chorus_max_intensity">Chorus maximum intensity</label>
        <input type="range" name="chorus_max_intensity" value="0.05" step="0.01" min="0" max="1.0" data-bind data-bind-type="float" />
        <small><span data-display-value-for="chorus_max_intensity" data-display-format="percent"></span> percent</small>
      </div>
      <aside>
        This setting controls the <strong>maximum</strong> intensity of the chorusing feature. The intensity is the amount that the internal <abbr title="Low-frequency oscillator">LFO</abbr> affects Pollux's pitch.
      </aside>
      <div class="column field">
        <label for="lfo_1_frequency">LFO frequency</label>
        <input type="range" name="lfo_1_frequency" value="0.2" step="0.1" min="0.1" max="10.0" data-bind data-bind-type="float"/>
        <small><span data-display-value-for="lfo_1_frequency"></span> hertz</small>
      </div>
      <aside>
          This setting controls the <strong>default</strong> frequency of the internal <abbr title="Low-frequency oscillator">LFO</abbr>. This LFO is used by the chorusing feature and the <abbr title="Pulse width modulation">PWM</abbr> feature. The frequency can be changed anytime by holding the <em>hard sync</em> button and moving the <em>chorusing amount</em> (<code>φ</code>) knob, this just sets the frequency at start-up.
      </aside>
      <div>
        <label class="label">LFO waveform</label>
        <canvas id="lfo-waveform-canvas" width="640" height="200" class="waveform teal"></canvas>
        <div class="grid col-2">
          <div>
            <label for="lfo_1_waveshape">Shape 1</label>
            <select id="lfo_1_waveshape" name="lfo_1_waveshape" data-bind data-bind-type="int">
              <option value="0">Triangle</option>
              <option value="1">Sine</option>
              <option value="2">Sawtooth</option>
              <option value="3">Square</option>
            </select>
            <label for="lfo_1_factor">Amplitude</label>
            <input type="range" id="lfo_1_factor" name="lfo_1_factor" value="1.0" step="0.01" min="0.0" max="1.0" data-bind data-bind-type="float"/>
          </div>
          <div>
            <label for="lfo_2_waveshape">Shape 2</label>
            <select id="lfo_2_waveshape" name="lfo_2_waveshape" data-bind data-bind-type="int">
              <option value="0">Triangle</option>
              <option value="1">Sine</option>
              <option value="2">Sawtooth</option>
              <option value="3">Square</option>
            </select>
            <label for="lfo_2_factor">Amplitude</label>
            <input type="range" id="lfo_2_factor" name="lfo_2_factor" value="1.0" step="0.01" min="0.0" max="1.0" data-bind data-bind-type="float"/>
            <label for="lfo_2_frequency_ratio">Frequency ratio</label>
            <input type="range" id="lfo_2_frequency_ratio" name="lfo_2_frequency_ratio" value="1.0" step="0.01" min="0.0" max="10.0" data-bind data-bind-type="float"/>
          </div>
        </div>
      </div>
      <aside>
          These settings control the overall waveshape of the internal <abbr title="Low-frequency oscillator">LFO</abbr>. The LFO is generated by adding two separate waveforms together. By default, the first waveform is a triangle wave and the second waveform is turned off.
      </aside>
      <div>
        <label for="pulse_width_bitmask">Pulse width behavior</label>
        <select id="pulse_width_bitmask" name="pulse_width_bitmask" data-bind data-bind-type="int">
            <option value="4095">Smooth</option>
            <option value="3840">Classic</option>
        </select>
      </div>
      <aside>
          This controls the behavior of the pulse wave when the pulse width is modulated. Smooth is almost certainly what you want, however, if you choose Classic you'll introduce some interesting glitchy/stepping behavior when the pulse width changes.
          <br>
          The reason this setting exists is because prior to the July 2022 firmware release, C&P had a bug that caused the effective pulse width resolution to be far lower than intended. Some folks like the old sound, so we added this setting for nostalgia.
      </aside>
    </div>
    <button type="submit" id="save_button">Save &amp; restart</button>
  </section><!-- settings_section -->
  <!-- Dangerous settings -->
  <section id="danger_section" hidden>
    <h2>Danger zone</h2>
    <p>These settings are calibrated during assembly and <strong>do not have default values</strong>. Changing these settings can cause your module to behave incorrectly, so do not touch them unless you know what you're doing or have been asked to do so by support. It's highly recommended that you write down these settings before making changes in case you need to restore them.</p>
    <label for="allow_danger">
      <input class="checkbox" type="checkbox" id="allow_danger" />
      I understand, let me in!
    </label>
    <div id="danger_section_content" class="grid col-2" hidden>
      <div>
        <label for="osc8m_freq">8 MHz clock measured frequency</label>
        <input type="number" name="osc8m_freq" min="7000000" max="9000000" value="8000000" data-bind data-bind-type="int"/>
        <small>Hertz</small>
      </div>
      <aside>
          C&P uses an internal <code>8 MHz</code> clock to generate the frequency signals for the oscillators. The actual frequency of this oscillator can vary between <code>7.94 MHz</code> and <code>8.06 MHz</code>. This setting is set to compensate for that so that the oscillator outputs are in tune.
      </aside>
      <div>
        <label for="cv_gain_error">CV input gain correction</label>
        <input type="number" name="cv_gain_error" value="1" min="0.5" max="2.0" step="0.0001" data-bind data-bind-type="float" data-binding-precision="4"/>
        <small>multiplier</small>
        <label for="cv_offset_error">CV input error correction</label>
        <input type="number" name="cv_offset_error" value="0" data-bind-type="float" data-bind data-binding-precision="1"/>
        <small>code points</small>
        <label for="adc_gain_corr">ADC gain correction</label>
        <input type="number" name="adc_gain_corr" value="2048" min="1024" max="3072" data-bind data-bind-type="int"/>
        <small><span data-display-value-for="adc_gain_corr" data-display-formatter="(input.valueAsNumber / 2048).toFixed(3)"></span>x multiplier</small>
        <label for="adc_offset_corr">ADC offset correction</label>
        <input type="number" name="adc_offset_corr" value="0" min="-100" max="100" data-bind data-bind-type="int" />
        <small>code points</small>
      </div>
      <aside>
        <p>
          These settings determine how the module corrects for gain and offset errors in its <abbr title="Analog to digital converter">ADC</abbr>. We calibrate every unit during manufacturing and save the calibration data to the device. However, we keep a copy of it in case it somehow gets erased or corrupted. You can use the restore button below to load the values that were originally on your device. If the button is greyed out, then we don't have a copy of your module's calibration data. ☹️
        </p>
        <button class="outline" type="button" id="restore_adc_calibration">Restore to factory values</button>
      </aside>
      <div>
        <label>Tuning calibration</label>
        <div class="grid col-2">
          <button type="button" class="secondary" id="measure_one">Measure 1V</button>
          <input id="measure_one_result" type="number" placeholder="not measured" readonly>
          <button type="button" class="secondary" id="measure_three">Measure 3V</button>
          <input id="measure_three_result" type="number" placeholder="not measured" readonly>
          <div>
            <label for="gain_error_result">Gain error</label>
            <input id="gain_error_result" name="gain_error_result" type="number" readonly/>
          </div>
          <div>
            <label for="offset_error_result">Offset error</label>
            <input id="offset_error_result" name="offset_error_result" type="number" readonly/>
          </div>
        </div>
        <button type="button" class="danger outline" id="save_tuning" disabled>Save</button>
      </div>
      <aside>
        This allows you to re-calibrate your module's tuning. You should only do this if you absolutely know what you're doing or have been instructed to do so by support. <strong>Please write down the values for the settings in the previous section</strong> as this will overwrite them.
        <br>
        To use this, connect the most accurate <abbr title="Control voltage">CV</abbr> source that you have to the first oscillator's pitch input. Set the CV to <code>1v</code> and click <kbd>Measure 1V</kbd>. Repeat for <code>3v</code>. Once both measurements have been taken, it will calculate new values for tuning calibration. Click <kbd>Save</kbd> to write these to the device.
      </aside>
    </div>
  </section><!-- danger_section -->
  <section id="monitor_section" hidden>
    <h2>Monitor</h2>
    <p>This is an advanced debugging tool. You shouldn't even see this unless you know what you're doing!</p>
    <button class="outline danger" type="button" id="enable_monitor">Enable monitoring</button>
    <pre class="code" id="monitor_output">monitor output</pre>
  </section>
  <section id="ramp_section" hidden>
    <h2>Ramp calibration</h2>
    <button class="button is-warning" type="button" id="ramp_swap">Swap ramp calibration</button>
    <pre class="code" id="ramp_output">Output will appear here</pre>
  </section>
</form>

<script type="module" src="../scripts/settings.js"></script>
