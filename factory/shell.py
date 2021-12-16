import statistics
import time

import IPython
import wintertools.oscilloscope

from libgemini import fallback_calibration, gemini, oscillators, reference_calibration

gem = gemini.Gemini.get()


def set_oscillators_to_note(note, calibration=reference_calibration):
    freq = oscillators.midi_note_to_frequency(note)
    period = oscillators.frequency_to_timer_period(freq)

    charge_code_castor = oscillators.calibrated_charge_code_for_period(
        period, calibration.castor
    )
    charge_code_pollux = oscillators.calibrated_charge_code_for_period(
        period, calibration.pollux
    )

    print(
        f"Note: {note}, Freq: {freq}, Charge codes: {charge_code_castor}, {charge_code_pollux}"
    )

    gem.set_dac(charge_code_castor, 2048, charge_code_pollux, 2048)
    gem.set_period(0, period)
    gem.set_period(1, period)


def sweep_notes(calibration=reference_calibration):
    for n in range(12, 94):
        set_oscillators_to_note(n, calibration=reference_calibration)
        time.sleep(0.5)


def sweep_notes_with_ref():
    return sweep_notes(calibration=reference_calibration)


def sweep_notes_with_fallback():
    return sweep_notes(calibration=fallback_calibration)


def get_oscilloscope():
    return wintertools.oscilloscope.Oscilloscope()


def read_adc_average(channel, count=100):
    return statistics.mean([gem.read_adc(channel) for _ in range(count)])


gem.enter_calibration_mode()

IPython.embed()
