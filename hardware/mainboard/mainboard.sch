EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 7
Title "Castor & Pollux"
Date "2021-02-01"
Rev "v3"
Comp "Winterbloom"
Comment1 "CERN-OHL-P v2"
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
L Connector_Generic:Conn_01x20 J4
U 1 1 5F4A7F99
P 2400 6750
F 0 "J4" V 2525 6696 50  0000 C CNN
F 1 "left header" V 2616 6696 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 2400 6750 50  0001 C CNN
F 3 "~" H 2400 6750 50  0001 C CNN
F 4 "yes" H 2400 6750 50  0001 C CNN "finalized"
F 5 "4UCON 21790" H 2400 6750 50  0001 C CNN "mpn"
F 6 "" H 2400 6750 50  0001 C CNN "mpn alt"
F 7 "1" H 2400 6750 50  0001 C CNN "dnp"
	1    2400 6750
	1    0    0    1   
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
F18 "jack_detection_probe" I R 2800 3150 50 
F19 "hard_sync" I R 2800 2850 50 
F20 "chorus_pot" I R 2800 2750 50 
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
Wire Wire Line
	1100 3900 1300 3900
Wire Wire Line
	1100 4000 1300 4000
Wire Wire Line
	1300 4100 1100 4100
Wire Wire Line
	1100 4200 1300 4200
Text Label 3250 2150 0    50   ~ 0
cv_a_pot
Text Label 3250 2250 0    50   ~ 0
cv_b_pot
Text Label 3250 2350 0    50   ~ 0
duty_a_pot
Text Label 3250 2450 0    50   ~ 0
duty_b_pot
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
Text Label 3600 7350 0    50   ~ 0
mix_pot_out
Text Label 3600 7450 0    50   ~ 0
mix_pot_in
Text Label 3600 7550 0    50   ~ 0
mix_out
Text Label 3600 6350 0    50   ~ 0
cv_a_pot
Text Label 3600 6450 0    50   ~ 0
cv_b_pot
Text Label 3600 7050 0    50   ~ 0
cv_b_jack
Text Label 3600 6950 0    50   ~ 0
cv_a_jack
$Comp
L power:GND #PWR0136
U 1 1 5F5062C4
P 2200 5750
F 0 "#PWR0136" H 2200 5500 50  0001 C CNN
F 1 "GND" V 2200 5550 50  0000 C CNN
F 2 "" H 2200 5750 50  0001 C CNN
F 3 "" H 2200 5750 50  0001 C CNN
	1    2200 5750
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x20 J5
U 1 1 5F5071F3
P 3100 6750
F 0 "J5" V 3225 6696 50  0000 C CNN
F 1 "right header" V 3316 6696 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 3100 6750 50  0001 C CNN
F 3 "~" H 3100 6750 50  0001 C CNN
F 4 "yes" H 3100 6750 50  0001 C CNN "finalized"
F 5 "4UCON 21790" H 3100 6750 50  0001 C CNN "mpn"
F 6 "" H 3100 6750 50  0001 C CNN "mpn alt"
F 7 "1" H 3100 6750 50  0001 C CNN "dnp"
	1    3100 6750
	-1   0    0    1   
$EndComp
Text Label 1950 6650 2    50   ~ 0
pollux_sub_mix_in
Text Label 1950 6550 2    50   ~ 0
pollux_pwm_mix_in
Text Label 1950 6450 2    50   ~ 0
pollux_ramp_mix_in
Text Label 1950 6750 2    50   ~ 0
pollux_mix_out
Text Label 1950 6350 2    50   ~ 0
pollux_sub_out
Text Label 1950 6250 2    50   ~ 0
pollux_pwm_out
Text Label 1950 6150 2    50   ~ 0
pollux_ramp_out
Wire Wire Line
	1950 7250 2200 7250
Wire Wire Line
	1950 7150 2200 7150
Wire Wire Line
	2200 7050 1950 7050
Wire Wire Line
	1950 6850 2200 6850
Text Label 1950 7450 2    50   ~ 0
castor_mix_out
Text Label 1950 7050 2    50   ~ 0
castor_sub_out
Text Label 1950 6950 2    50   ~ 0
castor_pwm_out
Text Label 1950 6850 2    50   ~ 0
castor_ramp_out
Wire Wire Line
	1950 7350 2200 7350
Wire Wire Line
	1950 6950 2200 6950
Wire Wire Line
	1950 7450 2200 7450
Text Label 1950 7350 2    50   ~ 0
castor_sub_mix_in
Text Label 1950 7250 2    50   ~ 0
castor_pwm_mix_in
Text Label 1950 7150 2    50   ~ 0
castor_ramp_mix_in
Wire Wire Line
	1950 6650 2200 6650
Wire Wire Line
	2200 6750 1950 6750
Wire Wire Line
	1950 6350 2200 6350
Wire Wire Line
	1950 6550 2200 6550
Wire Wire Line
	2200 6450 1950 6450
Wire Wire Line
	1950 6250 2200 6250
Wire Wire Line
	2200 6150 1950 6150
Wire Wire Line
	3600 7350 3300 7350
Wire Wire Line
	3600 7550 3300 7550
Wire Wire Line
	3600 7450 3300 7450
$Comp
L power:GND #PWR0142
U 1 1 5F566FE6
P 3300 5750
F 0 "#PWR0142" H 3300 5500 50  0001 C CNN
F 1 "GND" V 3300 5550 50  0000 C CNN
F 2 "" H 3300 5750 50  0001 C CNN
F 3 "" H 3300 5750 50  0001 C CNN
	1    3300 5750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3600 6350 3300 6350
Wire Wire Line
	3300 6450 3600 6450
Text Label 1950 5850 2    50   ~ 0
-12v-raw
Text Label 1950 5950 2    50   ~ 0
-12v-raw
Text Label 9000 4450 0    50   ~ 0
+12v-raw
Text Label 3550 5850 0    50   ~ 0
+12v-raw
Text Label 9000 4600 0    50   ~ 0
-12v-raw
Wire Wire Line
	9000 4450 8750 4450
Wire Wire Line
	8750 4600 9000 4600
Text Label 3550 5950 0    50   ~ 0
+12v-raw
Text Label 3050 850  0    50   ~ 0
dotstar_data
Text Label 3050 950  0    50   ~ 0
dotstar_clk
Wire Wire Line
	2800 850  3050 850 
Wire Wire Line
	2800 950  3050 950 
Text Label 3550 6150 0    50   ~ 0
dotstar_data
Text Label 3550 6250 0    50   ~ 0
dotstar_clk
$Comp
L power:+3.3V #PWR0146
U 1 1 5F6BDFA3
P 3550 6050
F 0 "#PWR0146" H 3550 5900 50  0001 C CNN
F 1 "+3.3V" V 3550 6300 50  0000 C CNN
F 2 "" H 3550 6050 50  0001 C CNN
F 3 "" H 3550 6050 50  0001 C CNN
	1    3550 6050
	0    1    1    0   
$EndComp
$Comp
L power:+3.3VA #PWR0147
U 1 1 5F6BED82
P 1950 6050
F 0 "#PWR0147" H 1950 5900 50  0001 C CNN
F 1 "+3.3VA" V 1950 6300 50  0000 C CNN
F 2 "" H 1950 6050 50  0001 C CNN
F 3 "" H 1950 6050 50  0001 C CNN
	1    1950 6050
	0    -1   1    0   
$EndComp
Wire Wire Line
	2800 1450 4500 1450
Wire Wire Line
	4500 1450 4500 1400
Wire Wire Line
	2800 1550 4850 1550
Wire Wire Line
	4150 2800 4150 1650
Wire Wire Line
	4150 1650 2800 1650
Wire Wire Line
	2800 1750 4100 1750
Wire Wire Line
	4100 1750 4100 2950
Wire Wire Line
	3550 5850 3300 5850
Wire Wire Line
	3550 5950 3300 5950
Wire Wire Line
	3550 6050 3300 6050
Wire Wire Line
	3550 6150 3300 6150
Wire Wire Line
	3550 6250 3300 6250
Wire Wire Line
	1950 5850 2200 5850
Wire Wire Line
	2200 5950 1950 5950
Wire Wire Line
	1950 6050 2200 6050
Wire Wire Line
	3600 7050 3300 7050
Wire Wire Line
	3300 7250 3600 7250
Wire Wire Line
	3600 6850 3300 6850
Wire Wire Line
	3600 6550 3300 6550
Wire Wire Line
	3300 6950 3600 6950
Wire Wire Line
	3600 7150 3300 7150
Text Label 3600 7150 0    50   ~ 0
hard_sync
Text Label 3600 6850 0    50   ~ 0
duty_b_jack
Text Label 3600 7250 0    50   ~ 0
duty_a_jack
Text Label 3600 6550 0    50   ~ 0
chorus_pot
Text Label 3600 6750 0    50   ~ 0
duty_b_pot
Text Label 3600 6650 0    50   ~ 0
duty_a_pot
$Comp
L power:GND #PWR0137
U 1 1 5F3D4789
P 3300 7650
F 0 "#PWR0137" H 3300 7400 50  0001 C CNN
F 1 "GND" V 3300 7450 50  0000 C CNN
F 2 "" H 3300 7650 50  0001 C CNN
F 3 "" H 3300 7650 50  0001 C CNN
	1    3300 7650
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0138
U 1 1 5F3D4BB0
P 2200 7650
F 0 "#PWR0138" H 2200 7400 50  0001 C CNN
F 1 "GND" V 2200 7450 50  0000 C CNN
F 2 "" H 2200 7650 50  0001 C CNN
F 3 "" H 2200 7650 50  0001 C CNN
	1    2200 7650
	0    1    -1   0   
$EndComp
Wire Wire Line
	3600 6750 3300 6750
Wire Wire Line
	3600 6650 3300 6650
$Comp
L Mechanical:MountingHole_Pad MH1
U 1 1 5F48E605
P 6350 6200
F 0 "MH1" H 6450 6249 50  0000 L CNN
F 1 "M3" H 6450 6158 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 6350 6200 50  0001 C CNN
F 3 "~" H 6350 6200 50  0001 C CNN
F 4 "1" H 6350 6200 50  0001 C CNN "dnp"
F 5 "yes" H 6350 6200 50  0001 C CNN "finalized"
	1    6350 6200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0140
U 1 1 5F48F1EB
P 6350 6300
F 0 "#PWR0140" H 6350 6050 50  0001 C CNN
F 1 "GND" H 6355 6127 50  0000 C CNN
F 2 "" H 6350 6300 50  0001 C CNN
F 3 "" H 6350 6300 50  0001 C CNN
	1    6350 6300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0141
U 1 1 5F22906B
P 2200 7550
F 0 "#PWR0141" H 2200 7300 50  0001 C CNN
F 1 "GND" V 2200 7350 50  0000 C CNN
F 2 "" H 2200 7550 50  0001 C CNN
F 3 "" H 2200 7550 50  0001 C CNN
	1    2200 7550
	0    1    -1   0   
$EndComp
Text Label 3450 2750 0    50   ~ 0
chorus_pot
Text Label 3500 2850 0    50   ~ 0
hard_sync
$EndSCHEMATC
