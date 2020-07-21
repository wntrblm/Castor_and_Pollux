EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 7
Title "Castor & Pollux"
Date "2020-07-04"
Rev "v0"
Comp "Winterbloom"
Comment1 "CC BY-SA 4.0"
Comment2 "Alethea Flowers"
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 7450 4000 1300 1100
U 5F01694C
F0 "Power" 50
F1 "power.sch" 50
F2 "-12v-raw" I R 8750 4600 50 
F3 "+12v-raw" I R 8750 4450 50 
$EndSheet
$Comp
L Connector_Generic:Conn_01x24 J4
U 1 1 5F4A7F99
P 4350 7450
F 0 "J4" V 4475 7396 50  0000 C CNN
F 1 "bottom header" V 4566 7396 50  0000 C CNN
F 2 "MF_Connectors:MF_Connectors-PTH_2.54MM_01X24" H 4350 7450 50  0001 C CNN
F 3 "~" H 4350 7450 50  0001 C CNN
	1    4350 7450
	0    1    1    0   
$EndComp
$Sheet
S 7600 1750 1350 1150
U 5F0D50CE
F0 "Output Mixer" 50
F1 "outputmixer.sch" 50
F2 "castor_in" I L 7600 1950 50 
F3 "pollux_in" I L 7600 2700 50 
F4 "mix_pot_out" I R 8950 2200 50 
F5 "mix_pot_in" I R 8950 2350 50 
F6 "mix_out" I R 8950 2500 50 
$EndSheet
$Sheet
S 4850 2550 1350 1050
U 5F0E55E2
F0 "Pollux" 50
F1 "castor.sch" 50
F2 "square_in" I L 4850 2650 50 
F3 "charge_cv_in" I L 4850 2800 50 
F4 "pwm_cv_in" I L 4850 2950 50 
F5 "ramp_out" I R 6200 2650 50 
F6 "pwm_out" I R 6200 2800 50 
F7 "sub_out" I R 6200 2950 50 
F8 "mix_out" I R 6200 3100 50 
F9 "ramp_mix_in" I L 4850 3200 50 
F10 "pwm_mix_in" I L 4850 3350 50 
F11 "sub_mix_in" I L 4850 3500 50 
$EndSheet
Wire Wire Line
	2800 1250 4850 1250
Wire Wire Line
	4200 1350 4200 2650
Wire Wire Line
	4200 2650 4850 2650
Wire Wire Line
	4500 1400 4850 1400
Wire Wire Line
	4150 2800 4850 2800
Wire Wire Line
	4100 2950 4850 2950
Wire Wire Line
	2800 1350 4200 1350
Wire Wire Line
	4150 1450 2800 1450
Wire Wire Line
	4150 1450 4150 2800
Wire Wire Line
	2800 1550 4100 1550
Wire Wire Line
	4100 1550 4100 2950
Wire Wire Line
	4500 1650 2800 1650
Wire Wire Line
	4500 1400 4500 1650
Wire Wire Line
	2800 1750 4600 1750
Wire Wire Line
	4600 1750 4600 1550
Wire Wire Line
	4600 1550 4850 1550
$Sheet
S 1300 3700 1650 1300
U 5F3674A8
F0 "Analog Ins" 50
F1 "analogins.sch" 50
F2 "cv_a_jack" I L 1300 4000 50 
F3 "cv_a_scaled" I R 2950 3900 50 
F4 "cv_b_scaled" I R 2950 4000 50 
F5 "duty_a_scaled" I R 2950 4100 50 
F6 "duty_a_jack" I L 1300 4100 50 
F7 "cv_b_jack" I L 1300 3900 50 
F8 "duty_b_scaled" I R 2950 4200 50 
F9 "duty_b_jack" I L 1300 4200 50 
F10 "phase_jack" I L 1300 4300 50 
F11 "phase_scaled" I R 2950 4300 50 
$EndSheet
Wire Wire Line
	6200 1700 6550 1700
Wire Wire Line
	6550 1700 6550 1950
Wire Wire Line
	7400 2700 7600 2700
Wire Wire Line
	2950 3900 3050 3900
Wire Wire Line
	3050 3900 3050 1950
Wire Wire Line
	3050 1950 2800 1950
Wire Wire Line
	2950 4000 3150 4000
Wire Wire Line
	3150 4000 3150 2050
Wire Wire Line
	3150 2050 2800 2050
Wire Wire Line
	2950 4100 3250 4100
Wire Wire Line
	3250 4100 3250 2550
Wire Wire Line
	3250 2550 2800 2550
Wire Wire Line
	2950 4200 3350 4200
Wire Wire Line
	3350 4200 3350 2650
Wire Wire Line
	3350 2650 2800 2650
Wire Wire Line
	2950 4300 3450 4300
Wire Wire Line
	3450 4300 3450 2750
Wire Wire Line
	3450 2750 2800 2750
$Sheet
S 1250 750  1550 2500
U 5F09EAA6
F0 "MCU" 50
F1 "mcu.sch" 50
F2 "CV_A" I R 2800 1450 50 
F3 "CV_B" I R 2800 1550 50 
F4 "CV_C" I R 2800 1650 50 
F5 "CV_D" I R 2800 1750 50 
F6 "square_out_a" O R 2800 1250 50 
F7 "square_out_b" I R 2800 1350 50 
F8 "dotstar_data" I R 2800 850 50 
F9 "dotstar_clk" I R 2800 950 50 
F10 "cv_a_in" I R 2800 1950 50 
F11 "cv_b_in" I R 2800 2050 50 
F12 "cv_a_pot" I R 2800 2150 50 
F13 "cv_b_pot" I R 2800 2250 50 
F14 "duty_a_pot" I R 2800 2350 50 
F15 "duty_b_pot" I R 2800 2450 50 
F16 "duty_a_in" I R 2800 2550 50 
F17 "duty_b_in" I R 2800 2650 50 
F18 "phase_in" I R 2800 2750 50 
F19 "phase_pot" I R 2800 2850 50 
F20 "jack_detection_probe" I R 2800 3150 50 
$EndSheet
$Sheet
S 4850 1150 1350 1100
U 5F016AD7
F0 "Castor" 50
F1 "castor.sch" 50
F2 "square_in" I L 4850 1250 50 
F3 "charge_cv_in" I L 4850 1400 50 
F4 "pwm_cv_in" I L 4850 1550 50 
F5 "ramp_out" I R 6200 1250 50 
F6 "pwm_out" I R 6200 1400 50 
F7 "sub_out" I R 6200 1550 50 
F8 "mix_out" I R 6200 1700 50 
F9 "ramp_mix_in" I L 4850 1850 50 
F10 "pwm_mix_in" I L 4850 2000 50 
F11 "sub_mix_in" I L 4850 2150 50 
$EndSheet
Text Label 1100 3900 2    50   ~ 0
cv_b_jack
Text Label 1100 4000 2    50   ~ 0
cv_a_jack
Text Label 1100 4100 2    50   ~ 0
duty_a_jack
Text Label 1100 4200 2    50   ~ 0
duty_b_jack
Text Label 1100 4300 2    50   ~ 0
phase_jack
Wire Wire Line
	1100 3900 1300 3900
Wire Wire Line
	1100 4000 1300 4000
Wire Wire Line
	1300 4100 1100 4100
Wire Wire Line
	1100 4200 1300 4200
Wire Wire Line
	1100 4300 1300 4300
Text Label 3250 2150 0    50   ~ 0
cv_a_pot
Text Label 3250 2250 0    50   ~ 0
cv_b_pot
Text Label 3250 2350 0    50   ~ 0
duty_a_pot
Text Label 3250 2450 0    50   ~ 0
duty_b_pot
Text Label 3500 2900 0    50   ~ 0
phase_pot
Wire Wire Line
	3250 2150 2800 2150
Wire Wire Line
	2800 2250 3250 2250
Wire Wire Line
	3250 2350 2800 2350
Wire Wire Line
	2800 2450 3250 2450
Wire Wire Line
	2800 2850 3500 2850
Wire Wire Line
	3500 2850 3500 2900
Text Label 4650 1850 2    50   ~ 0
castor_ramp_mix_in
Text Label 4650 2000 2    50   ~ 0
castor_pwm_mix_in
Text Label 4650 2150 2    50   ~ 0
castor_sub_mix_in
Text Label 4650 3200 2    50   ~ 0
pollux_ramp_mix_in
Text Label 4650 3350 2    50   ~ 0
pollux_pwm_mix_in
Text Label 4650 3500 2    50   ~ 0
pollux_sub_mix_in
Wire Wire Line
	4650 3500 4850 3500
Wire Wire Line
	4650 3350 4850 3350
Wire Wire Line
	4650 3200 4850 3200
Wire Wire Line
	4650 2150 4850 2150
Wire Wire Line
	4650 2000 4850 2000
Wire Wire Line
	4650 1850 4850 1850
Text Label 6400 1250 0    50   ~ 0
castor_ramp_out
Text Label 6400 1400 0    50   ~ 0
castor_pwm_out
Text Label 6400 1550 0    50   ~ 0
castor_sub_out
Text Label 6550 1700 0    50   ~ 0
castor_mix_out
Text Label 6450 2650 0    50   ~ 0
pollux_ramp_out
Text Label 6450 2800 0    50   ~ 0
pollux_pwm_out
Text Label 6450 2950 0    50   ~ 0
pollux_sub_out
Text Label 6450 3100 0    50   ~ 0
pollux_mix_out
Wire Wire Line
	6550 1950 7600 1950
Wire Wire Line
	7400 3100 7400 2700
Wire Wire Line
	6200 3100 7400 3100
Wire Wire Line
	6200 1250 6400 1250
Wire Wire Line
	6200 1400 6400 1400
Wire Wire Line
	6200 1550 6400 1550
Wire Wire Line
	6200 2650 6450 2650
Wire Wire Line
	6200 2800 6450 2800
Wire Wire Line
	6200 2950 6450 2950
Text Label 9050 2200 0    50   ~ 0
mix_pot_out
Text Label 9050 2350 0    50   ~ 0
mix_pot_in
Text Label 9050 2500 0    50   ~ 0
mix_out
Wire Wire Line
	8950 2500 9050 2500
Wire Wire Line
	8950 2350 9050 2350
Wire Wire Line
	8950 2200 9050 2200
Text Label 1950 6950 1    50   ~ 0
mix_pot_out
Text Label 2150 6950 1    50   ~ 0
mix_pot_in
Text Label 2050 6950 1    50   ~ 0
mix_out
Text Label 2250 6950 1    50   ~ 0
cv_a_pot
Text Label 4750 6950 1    50   ~ 0
cv_b_pot
Text Label 4850 6950 1    50   ~ 0
duty_a_pot
Text Label 5050 6950 1    50   ~ 0
duty_b_pot
Text Label 4950 6950 1    50   ~ 0
phase_pot
Text Label 2350 6950 1    50   ~ 0
cv_b_jack
Text Label 2450 6950 1    50   ~ 0
cv_a_jack
Text Label 5350 6950 1    50   ~ 0
duty_a_jack
Text Label 5250 6950 1    50   ~ 0
duty_b_jack
Text Label 5150 6950 1    50   ~ 0
phase_jack
$Comp
L power:GND #PWR0136
U 1 1 5F5062C4
P 3150 7250
F 0 "#PWR0136" H 3150 7000 50  0001 C CNN
F 1 "GND" H 3155 7077 50  0000 C CNN
F 2 "" H 3150 7250 50  0001 C CNN
F 3 "" H 3150 7250 50  0001 C CNN
	1    3150 7250
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0137
U 1 1 5F50641A
P 5450 7250
F 0 "#PWR0137" H 5450 7000 50  0001 C CNN
F 1 "GND" H 5455 7077 50  0000 C CNN
F 2 "" H 5450 7250 50  0001 C CNN
F 3 "" H 5450 7250 50  0001 C CNN
	1    5450 7250
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0138
U 1 1 5F506BA1
P 4250 7250
F 0 "#PWR0138" H 4250 7000 50  0001 C CNN
F 1 "GND" H 4255 7077 50  0000 C CNN
F 2 "" H 4250 7250 50  0001 C CNN
F 3 "" H 4250 7250 50  0001 C CNN
	1    4250 7250
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0139
U 1 1 5F5070A0
P 4350 7250
F 0 "#PWR0139" H 4350 7000 50  0001 C CNN
F 1 "GND" H 4355 7077 50  0000 C CNN
F 2 "" H 4350 7250 50  0001 C CNN
F 3 "" H 4350 7250 50  0001 C CNN
	1    4350 7250
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x14 J5
U 1 1 5F5071F3
P 1850 7450
F 0 "J5" V 1975 7396 50  0000 C CNN
F 1 "almost bottom header" V 2066 7396 50  0000 C CNN
F 2 "MF_Connectors:MF_Connectors-PTH_2.54MM_01X14" H 1850 7450 50  0001 C CNN
F 3 "~" H 1850 7450 50  0001 C CNN
	1    1850 7450
	0    1    1    0   
$EndComp
Text Label 1450 7000 1    50   ~ 0
pollux_sub_mix_in
Text Label 3950 7000 1    50   ~ 0
pollux_pwm_mix_in
Text Label 3850 7000 1    50   ~ 0
pollux_ramp_mix_in
Text Label 1550 7000 1    50   ~ 0
pollux_mix_out
Text Label 3750 7000 1    50   ~ 0
pollux_sub_out
Text Label 1650 7000 1    50   ~ 0
pollux_pwm_out
Text Label 3650 7000 1    50   ~ 0
pollux_ramp_out
Wire Wire Line
	3550 7000 3550 7250
Wire Wire Line
	3450 7000 3450 7250
Wire Wire Line
	3350 7250 3350 7000
Wire Wire Line
	3250 7000 3250 7250
Text Label 1350 7000 1    50   ~ 0
castor_mix_out
Text Label 3350 7000 1    50   ~ 0
castor_sub_out
Text Label 1250 7000 1    50   ~ 0
castor_pwm_out
Text Label 3250 7000 1    50   ~ 0
castor_ramp_out
Wire Wire Line
	1150 7000 1150 7250
Wire Wire Line
	1250 7000 1250 7250
Wire Wire Line
	1350 7000 1350 7250
Text Label 1150 7000 1    50   ~ 0
castor_sub_mix_in
Text Label 3550 7000 1    50   ~ 0
castor_pwm_mix_in
Text Label 3450 7000 1    50   ~ 0
castor_ramp_mix_in
Wire Wire Line
	1450 7000 1450 7250
Wire Wire Line
	1550 7250 1550 7000
Wire Wire Line
	1650 7000 1650 7250
Wire Wire Line
	3950 7000 3950 7250
Wire Wire Line
	3850 7250 3850 7000
Wire Wire Line
	3750 7000 3750 7250
Wire Wire Line
	3650 7250 3650 7000
Wire Wire Line
	1950 6950 1950 7250
Wire Wire Line
	2050 6950 2050 7250
Wire Wire Line
	2150 6950 2150 7250
$Comp
L power:GND #PWR0142
U 1 1 5F566FE6
P 1750 7250
F 0 "#PWR0142" H 1750 7000 50  0001 C CNN
F 1 "GND" H 1755 7077 50  0000 C CNN
F 2 "" H 1750 7250 50  0001 C CNN
F 3 "" H 1750 7250 50  0001 C CNN
	1    1750 7250
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0143
U 1 1 5F56759F
P 1850 7250
F 0 "#PWR0143" H 1850 7000 50  0001 C CNN
F 1 "GND" H 1855 7077 50  0000 C CNN
F 2 "" H 1850 7250 50  0001 C CNN
F 3 "" H 1850 7250 50  0001 C CNN
	1    1850 7250
	-1   0    0    1   
$EndComp
Wire Wire Line
	2350 6950 2350 7250
Wire Wire Line
	2450 7250 2450 6950
Wire Wire Line
	5150 6950 5150 7250
Wire Wire Line
	5250 7250 5250 6950
Wire Wire Line
	5350 6950 5350 7250
Wire Wire Line
	4750 7250 4750 6950
Wire Wire Line
	4850 6950 4850 7250
Wire Wire Line
	4950 7250 4950 6950
Wire Wire Line
	5050 6950 5050 7250
Wire Wire Line
	2250 6950 2250 7250
$Comp
L Connector_Generic:Conn_01x14 J1
U 1 1 5F5AB310
P 7600 6400
F 0 "J1" V 7725 6346 50  0000 C CNN
F 1 "top header" V 7816 6346 50  0000 C CNN
F 2 "MF_Connectors:MF_Connectors-PTH_2.54MM_01X14" H 7600 6400 50  0001 C CNN
F 3 "~" H 7600 6400 50  0001 C CNN
	1    7600 6400
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0144
U 1 1 5F5AEF8C
P 8200 6200
F 0 "#PWR0144" H 8200 5950 50  0001 C CNN
F 1 "GND" H 8205 6027 50  0000 C CNN
F 2 "" H 8200 6200 50  0001 C CNN
F 3 "" H 8200 6200 50  0001 C CNN
	1    8200 6200
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0145
U 1 1 5F5AFBFE
P 6900 6200
F 0 "#PWR0145" H 6900 5950 50  0001 C CNN
F 1 "GND" H 6905 6027 50  0000 C CNN
F 2 "" H 6900 6200 50  0001 C CNN
F 3 "" H 6900 6200 50  0001 C CNN
	1    6900 6200
	-1   0    0    1   
$EndComp
Text Label 8000 5950 1    50   ~ 0
-12v-raw
Text Label 8100 5950 1    50   ~ 0
-12v-raw
Wire Wire Line
	8100 5950 8100 6200
Wire Wire Line
	8000 6200 8000 5950
Text Label 9000 4450 0    50   ~ 0
+12v-raw
Text Label 7000 5950 1    50   ~ 0
+12v-raw
Wire Wire Line
	7000 5950 7000 6200
Wire Wire Line
	7100 5950 7100 6200
Text Label 9000 4600 0    50   ~ 0
-12v-raw
Wire Wire Line
	9000 4450 8750 4450
Wire Wire Line
	8750 4600 9000 4600
Text Label 7100 5950 1    50   ~ 0
+12v-raw
Text Label 3050 850  0    50   ~ 0
dotstar_data
Text Label 3050 950  0    50   ~ 0
dotstar_clk
Wire Wire Line
	2800 850  3050 850 
Wire Wire Line
	2800 950  3050 950 
Text Label 7300 5950 1    50   ~ 0
dotstar_data
Text Label 7200 5950 1    50   ~ 0
dotstar_clk
Wire Wire Line
	7200 5950 7200 6200
Wire Wire Line
	7300 6200 7300 5950
$Comp
L power:GND #PWR0140
U 1 1 5F6B45E0
P 7800 6200
F 0 "#PWR0140" H 7800 5950 50  0001 C CNN
F 1 "GND" H 7805 6027 50  0000 C CNN
F 2 "" H 7800 6200 50  0001 C CNN
F 3 "" H 7800 6200 50  0001 C CNN
	1    7800 6200
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0141
U 1 1 5F6B4E84
P 7900 6200
F 0 "#PWR0141" H 7900 5950 50  0001 C CNN
F 1 "GND" H 7905 6027 50  0000 C CNN
F 2 "" H 7900 6200 50  0001 C CNN
F 3 "" H 7900 6200 50  0001 C CNN
	1    7900 6200
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR0146
U 1 1 5F6BDFA3
P 7600 5950
F 0 "#PWR0146" H 7600 5800 50  0001 C CNN
F 1 "+3.3V" V 7600 6200 50  0000 C CNN
F 2 "" H 7600 5950 50  0001 C CNN
F 3 "" H 7600 5950 50  0001 C CNN
	1    7600 5950
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3VA #PWR0147
U 1 1 5F6BED82
P 7400 5950
F 0 "#PWR0147" H 7400 5800 50  0001 C CNN
F 1 "+3.3VA" V 7400 6200 50  0000 C CNN
F 2 "" H 7400 5950 50  0001 C CNN
F 3 "" H 7400 5950 50  0001 C CNN
	1    7400 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 5950 7400 6200
Wire Wire Line
	7600 6200 7600 5950
$Comp
L power:GND #PWR0148
U 1 1 5F6C4C01
P 7500 6200
F 0 "#PWR0148" H 7500 5950 50  0001 C CNN
F 1 "GND" H 7505 6027 50  0000 C CNN
F 2 "" H 7500 6200 50  0001 C CNN
F 3 "" H 7500 6200 50  0001 C CNN
	1    7500 6200
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0149
U 1 1 5F6C506F
P 7700 6200
F 0 "#PWR0149" H 7700 5950 50  0001 C CNN
F 1 "GND" H 7705 6027 50  0000 C CNN
F 2 "" H 7700 6200 50  0001 C CNN
F 3 "" H 7700 6200 50  0001 C CNN
	1    7700 6200
	-1   0    0    1   
$EndComp
$EndSCHEMATC
