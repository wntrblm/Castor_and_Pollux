# January 27, 2021 (mainboard v3)

This hardware revision was created to obtain better performance out of the ramp oscillators.

* Changed license from CC BY SA to CERN-OHL-P v2.
* Changed the oscillator behavior: Q1 was changed from 2N3094 (NPN) to 3N3906 (PNP). The previous design unintentionally used Q1 in reverse-active mode which had a significant impact on the maximum ramp voltage that could be achieved. Changing this to an appropriate PNP allows the transistor to always work in either cutoff or forward-active mode. This allows the ramp voltage to climb as high as the output rail on U3B (11.3v). This greatly improves headroom and lowers the amount of noise audible from the ramp oscillator.
* Modified U3C to be an inverting attenuator instead of an inverting amplifier. This is because previously the ramp's output was limited to about -500mV, but now the ramp's target voltage is 10v. This attenuator drops it down to +3.3v so that the ramp comparators continues to work as expected.
* Added R60 & R61 to limit the amount of current when C9/C28 is discharging through Q1/Q2.
* Changed the values of C6/C25, C9/C28, R2/R26, R3/R27, R4/R28 to match the Juno 106's values.
* Added ESD warning symbol.

# September 26th, 2020

* Initial hardware version
