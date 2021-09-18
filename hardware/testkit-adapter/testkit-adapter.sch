EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Castor & Pollux: testkit-adapter"
Date "2021-02-27"
Rev "v3"
Comp "Winterbloom"
Comment1 "gemini.wntr.dev"
Comment2 "CERN-OHL-P v2"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_01x20 J1
U 1 1 5F7A844F
P 1800 3600
F 0 "J1" H 1750 2350 50  0000 L CNN
F 1 "LEFT" H 1700 2450 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x20_P2.54mm_Vertical" H 1800 3600 50  0001 C CNN
F 3 "~" H 1800 3600 50  0001 C CNN
	1    1800 3600
	1    0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x20 J2
U 1 1 5F7A9E86
P 2150 3600
F 0 "J2" H 2068 2375 50  0000 C CNN
F 1 "RIGHT" H 2068 2466 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x20_P2.54mm_Vertical" H 2150 3600 50  0001 C CNN
F 3 "~" H 2150 3600 50  0001 C CNN
	1    2150 3600
	-1   0    0    1   
$EndComp
$Comp
L Connector:TestPoint TP4
U 1 1 5F7AF35E
P 10650 800
F 0 "TP4" H 10708 918 50  0000 L CNN
F 1 "CORE A" H 10708 827 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 10850 800 50  0001 C CNN
F 3 "~" H 10850 800 50  0001 C CNN
	1    10650 800 
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint TP5
U 1 1 5F7AF5B1
P 10650 1350
F 0 "TP5" H 10700 1475 50  0000 L CNN
F 1 "CORE B" H 10700 1375 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 10850 1350 50  0001 C CNN
F 3 "~" H 10850 1350 50  0001 C CNN
	1    10650 1350
	1    0    0    -1  
$EndComp
NoConn ~ 1050 1800
$Comp
L power:GND #PWR0106
U 1 1 5F7BD4F9
P 1600 2600
F 0 "#PWR0106" H 1600 2350 50  0001 C CNN
F 1 "GND" V 1605 2472 50  0001 R CNN
F 2 "" H 1600 2600 50  0001 C CNN
F 3 "" H 1600 2600 50  0001 C CNN
	1    1600 2600
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 5F7BD919
P 2350 2600
F 0 "#PWR0107" H 2350 2350 50  0001 C CNN
F 1 "GND" V 2355 2472 50  0001 R CNN
F 2 "" H 2350 2600 50  0001 C CNN
F 3 "" H 2350 2600 50  0001 C CNN
	1    2350 2600
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 5F7BDC66
P 2350 4500
F 0 "#PWR0108" H 2350 4250 50  0001 C CNN
F 1 "GND" V 2355 4372 50  0001 R CNN
F 2 "" H 2350 4500 50  0001 C CNN
F 3 "" H 2350 4500 50  0001 C CNN
	1    2350 4500
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5F7BE09E
P 1600 4500
F 0 "#PWR0109" H 1600 4250 50  0001 C CNN
F 1 "GND" V 1605 4372 50  0001 R CNN
F 2 "" H 1600 4500 50  0001 C CNN
F 3 "" H 1600 4500 50  0001 C CNN
	1    1600 4500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 5F7BE3B5
P 1600 4400
F 0 "#PWR0110" H 1600 4150 50  0001 C CNN
F 1 "GND" V 1605 4272 50  0001 R CNN
F 2 "" H 1600 4400 50  0001 C CNN
F 3 "" H 1600 4400 50  0001 C CNN
	1    1600 4400
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR0111
U 1 1 5F7BE88A
P 2350 2700
F 0 "#PWR0111" H 2350 2550 50  0001 C CNN
F 1 "+12V" V 2350 2800 50  0000 L CNN
F 2 "" H 2350 2700 50  0001 C CNN
F 3 "" H 2350 2700 50  0001 C CNN
	1    2350 2700
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR0112
U 1 1 5F7BF347
P 2350 2800
F 0 "#PWR0112" H 2350 2650 50  0001 C CNN
F 1 "+12V" V 2350 2900 50  0000 L CNN
F 2 "" H 2350 2800 50  0001 C CNN
F 3 "" H 2350 2800 50  0001 C CNN
	1    2350 2800
	0    1    1    0   
$EndComp
$Comp
L power:-12V #PWR0113
U 1 1 5F7BF606
P 1600 2700
F 0 "#PWR0113" H 1600 2800 50  0001 C CNN
F 1 "-12V" V 1615 2828 50  0000 L CNN
F 2 "" H 1600 2700 50  0001 C CNN
F 3 "" H 1600 2700 50  0001 C CNN
	1    1600 2700
	0    -1   -1   0   
$EndComp
$Comp
L power:-12V #PWR0114
U 1 1 5F7BFD34
P 1600 2800
F 0 "#PWR0114" H 1600 2900 50  0001 C CNN
F 1 "-12V" V 1615 2928 50  0000 L CNN
F 2 "" H 1600 2800 50  0001 C CNN
F 3 "" H 1600 2800 50  0001 C CNN
	1    1600 2800
	0    -1   -1   0   
$EndComp
Text Label 2350 3000 0    50   ~ 0
dotstar_data
Text Label 2350 3100 0    50   ~ 0
dotstar_clk
Text Label 2350 3200 0    50   ~ 0
cv_a_pot
$Comp
L LED:APA102 D1
U 1 1 5F7CDA78
P 2000 1450
F 0 "D1" H 2250 1800 50  0000 C CNN
F 1 "DOTSTAR" H 2250 1700 50  0000 C CNN
F 2 "LED_SMD:LED_RGB_5050-6" H 2050 1150 50  0001 L TNN
F 3 "http://www.led-color.com/upload/201506/APA102%20LED.pdf" H 2100 1075 50  0001 L TNN
	1    2000 1450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0119
U 1 1 5F7CF86E
P 2000 1750
F 0 "#PWR0119" H 2000 1500 50  0001 C CNN
F 1 "GND" H 2005 1577 50  0000 C CNN
F 2 "" H 2000 1750 50  0001 C CNN
F 3 "" H 2000 1750 50  0001 C CNN
	1    2000 1750
	1    0    0    -1  
$EndComp
NoConn ~ 2300 1350
NoConn ~ 2300 1450
Text Label 1700 1350 2    50   ~ 0
dotstar_data
Text Label 1700 1450 2    50   ~ 0
dotstar_clk
Text Label 2350 3800 0    50   ~ 0
cv_a_jack
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 5F7F591B
P 3650 7200
F 0 "H1" H 3550 7157 50  0000 R CNN
F 1 "M3" H 3550 7248 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 3650 7200 50  0001 C CNN
F 3 "~" H 3650 7200 50  0001 C CNN
	1    3650 7200
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0130
U 1 1 5F7F5D9E
P 3650 7100
F 0 "#PWR0130" H 3650 6850 50  0001 C CNN
F 1 "GND" H 3655 6927 50  0000 C CNN
F 2 "" H 3650 7100 50  0001 C CNN
F 3 "" H 3650 7100 50  0001 C CNN
	1    3650 7100
	-1   0    0    1   
$EndComp
Text Label 1600 3000 2    50   ~ 0
pollux_ramp
Text Label 1600 3100 2    50   ~ 0
pollux_pwm
Text Label 1600 3200 2    50   ~ 0
pollux_sub
Text Label 1600 3700 2    50   ~ 0
castor_ramp
Text Label 1600 3800 2    50   ~ 0
castor_pwm
Text Label 1600 3900 2    50   ~ 0
castor_sub
Text Label 2350 3400 0    50   ~ 0
chorus_pot
Text Label 2350 3300 0    50   ~ 0
cv_b_pot
Text Label 1600 3300 2    50   ~ 0
pollux_ramp_mix_in
Text Label 1600 3400 2    50   ~ 0
pollux_pwm_mix_in
Text Label 1600 3500 2    50   ~ 0
pollux_sub_mix_in
Text Label 1600 3600 2    50   ~ 0
pollux_mix_out
Text Label 1600 4000 2    50   ~ 0
castor_ramp_mix_in
Text Label 1600 4100 2    50   ~ 0
castor_pwm_mix_in
Text Label 1600 4200 2    50   ~ 0
castor_sub_mix_in
Text Label 1600 4300 2    50   ~ 0
castor_mix_out
Text Label 2350 3500 0    50   ~ 0
duty_a_pot
Text Label 2350 3600 0    50   ~ 0
duty_b_pot
Text Label 2350 3700 0    50   ~ 0
duty_b_jack
Text Label 2350 3900 0    50   ~ 0
cv_b_jack
Text Label 2350 4000 0    50   ~ 0
button
Text Label 2350 4100 0    50   ~ 0
duty_a_jack
Text Label 2350 4200 0    50   ~ 0
mix_pot_out
Text Label 2350 4300 0    50   ~ 0
mix_pot_in
Text Label 2350 4400 0    50   ~ 0
mix_out
Text Label 4500 2900 2    50   ~ 0
mix_pot_out
Text Label 4500 2900 0    50   ~ 0
mix_pot_in
$Comp
L power:GND #PWR0103
U 1 1 5F7BA554
P 1050 7550
F 0 "#PWR0103" H 1050 7300 50  0001 C CNN
F 1 "GND" H 1055 7377 50  0001 C CNN
F 2 "" H 1050 7550 50  0001 C CNN
F 3 "" H 1050 7550 50  0001 C CNN
	1    1050 7550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5F7B9CB9
P 1150 7550
F 0 "#PWR0102" H 1150 7300 50  0001 C CNN
F 1 "GND" H 1155 7377 50  0001 C CNN
F 2 "" H 1150 7550 50  0001 C CNN
F 3 "" H 1150 7550 50  0001 C CNN
	1    1150 7550
	1    0    0    -1  
$EndComp
NoConn ~ 1650 7150
NoConn ~ 1650 7050
$Comp
L Connector:Conn_ARM_JTAG_SWD_10 J3
U 1 1 5F7B2421
P 1150 6950
F 0 "J3" H 707 6996 50  0000 R CNN
F 1 "SWD" H 707 6905 50  0000 R CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_2x05_P1.27mm_Vertical" H 1150 6950 50  0001 C CNN
F 3 "http://infocenter.arm.com/help/topic/com.arm.doc.ddi0314h/DDI0314H_coresight_components_trm.pdf" V 800 5700 50  0001 C CNN
	1    1150 6950
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint TP3
U 1 1 5F7A77BC
P 1650 6650
F 0 "TP3" V 1604 6838 50  0000 L CNN
F 1 "RESET" V 1695 6838 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 1850 6650 50  0001 C CNN
F 3 "~" H 1850 6650 50  0001 C CNN
	1    1650 6650
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP2
U 1 1 5F7A6769
P 1650 6950
F 0 "TP2" V 1604 7138 50  0000 L CNN
F 1 "SWDIO" V 1695 7138 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 1850 6950 50  0001 C CNN
F 3 "~" H 1850 6950 50  0001 C CNN
	1    1650 6950
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP1
U 1 1 5F7A63F2
P 1650 6850
F 0 "TP1" V 1604 7038 50  0000 L CNN
F 1 "SWCLK" V 1695 7038 50  0000 L CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 1850 6850 50  0001 C CNN
F 3 "~" H 1850 6850 50  0001 C CNN
	1    1650 6850
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J4
U 1 1 614470EB
P 5950 5125
F 0 "J4" H 6000 6242 50  0000 C CNN
F 1 "Left" H 6000 6151 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x20_P2.54mm_Vertical" H 5950 5125 50  0001 C CNN
F 3 "~" H 5950 5125 50  0001 C CNN
	1    5950 5125
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J5
U 1 1 6144B801
P 7950 5175
F 0 "J5" H 8000 6292 50  0000 C CNN
F 1 "Bottom" H 8000 6201 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x20_P2.54mm_Vertical" H 7950 5175 50  0001 C CNN
F 3 "~" H 7950 5175 50  0001 C CNN
	1    7950 5175
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J6
U 1 1 61451A2E
P 9950 5125
F 0 "J6" H 10000 6242 50  0000 C CNN
F 1 "Right" H 10000 6151 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x20_P2.54mm_Vertical" H 9950 5125 50  0001 C CNN
F 3 "~" H 9950 5125 50  0001 C CNN
	1    9950 5125
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 61464102
P 2925 6475
F 0 "H2" H 2825 6432 50  0000 R CNN
F 1 "M3" H 2825 6523 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2925 6475 50  0001 C CNN
F 3 "~" H 2925 6475 50  0001 C CNN
	1    2925 6475
	-1   0    0    1   
$EndComp
NoConn ~ 2925 6375
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 61465048
P 2925 6800
F 0 "H3" H 2825 6757 50  0000 R CNN
F 1 "M3" H 2825 6848 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2925 6800 50  0001 C CNN
F 3 "~" H 2925 6800 50  0001 C CNN
	1    2925 6800
	-1   0    0    1   
$EndComp
NoConn ~ 2925 6700
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 61465530
P 2925 7100
F 0 "H4" H 2825 7057 50  0000 R CNN
F 1 "M3" H 2825 7148 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2925 7100 50  0001 C CNN
F 3 "~" H 2925 7100 50  0001 C CNN
	1    2925 7100
	-1   0    0    1   
$EndComp
NoConn ~ 2925 7000
$Comp
L Mechanical:MountingHole_Pad H5
U 1 1 61465A06
P 2925 7375
F 0 "H5" H 2825 7332 50  0000 R CNN
F 1 "M3" H 2825 7423 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2925 7375 50  0001 C CNN
F 3 "~" H 2925 7375 50  0001 C CNN
	1    2925 7375
	-1   0    0    1   
$EndComp
NoConn ~ 2925 7275
$Comp
L power:GND #PWR0101
U 1 1 6143C724
P 6250 4225
F 0 "#PWR0101" H 6250 3975 50  0001 C CNN
F 1 "GND" H 6255 4052 50  0001 C CNN
F 2 "" H 6250 4225 50  0001 C CNN
F 3 "" H 6250 4225 50  0001 C CNN
	1    6250 4225
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 6143CE33
P 5750 4225
F 0 "#PWR0104" H 5750 3975 50  0001 C CNN
F 1 "GND" H 5755 4052 50  0001 C CNN
F 2 "" H 5750 4225 50  0001 C CNN
F 3 "" H 5750 4225 50  0001 C CNN
	1    5750 4225
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 6143D522
P 5750 4425
F 0 "#PWR0105" H 5750 4175 50  0001 C CNN
F 1 "GND" H 5755 4252 50  0001 C CNN
F 2 "" H 5750 4425 50  0001 C CNN
F 3 "" H 5750 4425 50  0001 C CNN
	1    5750 4425
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0115
U 1 1 6143D854
P 5750 4625
F 0 "#PWR0115" H 5750 4375 50  0001 C CNN
F 1 "GND" H 5755 4452 50  0001 C CNN
F 2 "" H 5750 4625 50  0001 C CNN
F 3 "" H 5750 4625 50  0001 C CNN
	1    5750 4625
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0117
U 1 1 6143DD92
P 5750 4825
F 0 "#PWR0117" H 5750 4575 50  0001 C CNN
F 1 "GND" H 5755 4652 50  0001 C CNN
F 2 "" H 5750 4825 50  0001 C CNN
F 3 "" H 5750 4825 50  0001 C CNN
	1    5750 4825
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0118
U 1 1 6143E0E9
P 6250 4425
F 0 "#PWR0118" H 6250 4175 50  0001 C CNN
F 1 "GND" H 6255 4252 50  0001 C CNN
F 2 "" H 6250 4425 50  0001 C CNN
F 3 "" H 6250 4425 50  0001 C CNN
	1    6250 4425
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0120
U 1 1 6143E590
P 6250 4625
F 0 "#PWR0120" H 6250 4375 50  0001 C CNN
F 1 "GND" H 6255 4452 50  0001 C CNN
F 2 "" H 6250 4625 50  0001 C CNN
F 3 "" H 6250 4625 50  0001 C CNN
	1    6250 4625
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0121
U 1 1 6143E8C6
P 6250 4825
F 0 "#PWR0121" H 6250 4575 50  0001 C CNN
F 1 "GND" H 6255 4652 50  0001 C CNN
F 2 "" H 6250 4825 50  0001 C CNN
F 3 "" H 6250 4825 50  0001 C CNN
	1    6250 4825
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0122
U 1 1 6143EB62
P 5750 5525
F 0 "#PWR0122" H 5750 5275 50  0001 C CNN
F 1 "GND" H 5755 5352 50  0001 C CNN
F 2 "" H 5750 5525 50  0001 C CNN
F 3 "" H 5750 5525 50  0001 C CNN
	1    5750 5525
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0123
U 1 1 6143F0FB
P 6250 5525
F 0 "#PWR0123" H 6250 5275 50  0001 C CNN
F 1 "GND" H 6255 5352 50  0001 C CNN
F 2 "" H 6250 5525 50  0001 C CNN
F 3 "" H 6250 5525 50  0001 C CNN
	1    6250 5525
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0124
U 1 1 6143F53D
P 5750 6125
F 0 "#PWR0124" H 5750 5875 50  0001 C CNN
F 1 "GND" H 5755 5952 50  0001 C CNN
F 2 "" H 5750 6125 50  0001 C CNN
F 3 "" H 5750 6125 50  0001 C CNN
	1    5750 6125
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0125
U 1 1 6143F9A3
P 6250 6125
F 0 "#PWR0125" H 6250 5875 50  0001 C CNN
F 1 "GND" H 6255 5952 50  0001 C CNN
F 2 "" H 6250 6125 50  0001 C CNN
F 3 "" H 6250 6125 50  0001 C CNN
	1    6250 6125
	0    -1   -1   0   
$EndComp
$Comp
L power:+12V #PWR0126
U 1 1 61440527
P 6250 4325
F 0 "#PWR0126" H 6250 4175 50  0001 C CNN
F 1 "+12V" V 6250 4425 50  0000 L CNN
F 2 "" H 6250 4325 50  0001 C CNN
F 3 "" H 6250 4325 50  0001 C CNN
	1    6250 4325
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR0127
U 1 1 61440DCE
P 5750 4325
F 0 "#PWR0127" H 5750 4175 50  0001 C CNN
F 1 "+12V" V 5750 4425 50  0000 L CNN
F 2 "" H 5750 4325 50  0001 C CNN
F 3 "" H 5750 4325 50  0001 C CNN
	1    5750 4325
	0    -1   -1   0   
$EndComp
$Comp
L power:-12V #PWR0128
U 1 1 61441D83
P 6250 4525
F 0 "#PWR0128" H 6250 4625 50  0001 C CNN
F 1 "-12V" V 6250 4625 50  0000 L CNN
F 2 "" H 6250 4525 50  0001 C CNN
F 3 "" H 6250 4525 50  0001 C CNN
	1    6250 4525
	0    1    1    0   
$EndComp
$Comp
L power:-12V #PWR0129
U 1 1 61442327
P 5750 4525
F 0 "#PWR0129" H 5750 4625 50  0001 C CNN
F 1 "-12V" V 5750 4625 50  0000 L CNN
F 2 "" H 5750 4525 50  0001 C CNN
F 3 "" H 5750 4525 50  0001 C CNN
	1    5750 4525
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR0131
U 1 1 614431C7
P 6250 4725
F 0 "#PWR0131" H 6250 4575 50  0001 C CNN
F 1 "+3.3V" V 6250 4825 50  0000 L CNN
F 2 "" H 6250 4725 50  0001 C CNN
F 3 "" H 6250 4725 50  0001 C CNN
	1    6250 4725
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR0132
U 1 1 61443808
P 5750 4725
F 0 "#PWR0132" H 5750 4575 50  0001 C CNN
F 1 "+3.3V" V 5750 4825 50  0000 L CNN
F 2 "" H 5750 4725 50  0001 C CNN
F 3 "" H 5750 4725 50  0001 C CNN
	1    5750 4725
	0    -1   -1   0   
$EndComp
Text GLabel 5750 4925 0    50   Input ~ 0
A1*
Text GLabel 6250 4925 2    50   Input ~ 0
A2*
Text GLabel 5750 5025 0    50   Input ~ 0
A3*
Text GLabel 5750 5125 0    50   Input ~ 0
A5*
Text GLabel 5750 5225 0    50   Input ~ 0
A7*
Text GLabel 5750 5325 0    50   Input ~ 0
A9*
Text GLabel 5750 5425 0    50   Input ~ 0
A11*
Text GLabel 6250 5025 2    50   Input ~ 0
A4*
Text GLabel 6250 5125 2    50   Input ~ 0
A6*
Text GLabel 6250 5225 2    50   Input ~ 0
A8*
Text GLabel 6250 5325 2    50   Input ~ 0
A10*
Text GLabel 6250 5425 2    50   Input ~ 0
A12*
Text GLabel 5750 5625 0    50   Input ~ 0
A13
Text GLabel 6250 5625 2    50   Input ~ 0
A14
Text GLabel 5750 5725 0    50   Input ~ 0
A15
Text GLabel 6250 5725 2    50   Input ~ 0
A16
Text GLabel 6250 5825 2    50   Input ~ 0
A17
Text GLabel 6250 5925 2    50   Input ~ 0
A19
Text GLabel 5750 5825 0    50   Input ~ 0
A18
Text GLabel 5750 5925 0    50   Input ~ 0
A20
$Comp
L power:GND #PWR0133
U 1 1 61446454
P 10250 4225
F 0 "#PWR0133" H 10250 3975 50  0001 C CNN
F 1 "GND" H 10255 4052 50  0001 C CNN
F 2 "" H 10250 4225 50  0001 C CNN
F 3 "" H 10250 4225 50  0001 C CNN
	1    10250 4225
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0134
U 1 1 614467EC
P 9750 4225
F 0 "#PWR0134" H 9750 3975 50  0001 C CNN
F 1 "GND" H 9755 4052 50  0001 C CNN
F 2 "" H 9750 4225 50  0001 C CNN
F 3 "" H 9750 4225 50  0001 C CNN
	1    9750 4225
	0    1    1    0   
$EndComp
Text GLabel 9750 4325 0    50   Input ~ 0
AUDIO_OUT_L
Text GLabel 10250 4325 2    50   Input ~ 0
AUDIO_OUT_R
Text GLabel 9750 4425 0    50   Input ~ 0
AUDIO_IN_2B
Text GLabel 9750 4525 0    50   Input ~ 0
AUDIO_IN_2C
Text GLabel 9750 4625 0    50   Input ~ 0
AUDIO_IN_1B
Text GLabel 9750 4725 0    50   Input ~ 0
AUDIO_IN_1C
Text GLabel 10250 4425 2    50   Input ~ 0
AUDIO_IN_2A
Text GLabel 10250 4525 2    50   Input ~ 0
AUDIO_IN_2D
Text GLabel 10250 4625 2    50   Input ~ 0
AUDIO_IN_1A
Text GLabel 10250 4725 2    50   Input ~ 0
AUDIO_IN_1D
$Comp
L power:GND #PWR0135
U 1 1 61447E71
P 9750 4825
F 0 "#PWR0135" H 9750 4575 50  0001 C CNN
F 1 "GND" H 9755 4652 50  0001 C CNN
F 2 "" H 9750 4825 50  0001 C CNN
F 3 "" H 9750 4825 50  0001 C CNN
	1    9750 4825
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0136
U 1 1 61448327
P 10250 4825
F 0 "#PWR0136" H 10250 4575 50  0001 C CNN
F 1 "GND" H 10255 4652 50  0001 C CNN
F 2 "" H 10250 4825 50  0001 C CNN
F 3 "" H 10250 4825 50  0001 C CNN
	1    10250 4825
	0    -1   -1   0   
$EndComp
Text GLabel 10250 5025 2    50   Input ~ 0
OSC_A
Text GLabel 9750 5025 0    50   Input ~ 0
OSC_B
Text GLabel 10250 4925 2    50   Input ~ 0
OSC_C
Text GLabel 9750 4925 0    50   Input ~ 0
OSC_D
$Comp
L power:GND #PWR0137
U 1 1 61448B87
P 9750 5125
F 0 "#PWR0137" H 9750 4875 50  0001 C CNN
F 1 "GND" H 9755 4952 50  0001 C CNN
F 2 "" H 9750 5125 50  0001 C CNN
F 3 "" H 9750 5125 50  0001 C CNN
	1    9750 5125
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0138
U 1 1 61448F26
P 10250 5125
F 0 "#PWR0138" H 10250 4875 50  0001 C CNN
F 1 "GND" H 10255 4952 50  0001 C CNN
F 2 "" H 10250 5125 50  0001 C CNN
F 3 "" H 10250 5125 50  0001 C CNN
	1    10250 5125
	0    -1   -1   0   
$EndComp
Text GLabel 10250 5225 2    50   Input ~ 0
DAC_2A
Text GLabel 10250 5325 2    50   Input ~ 0
DAC_2D
Text GLabel 10250 5425 2    50   Input ~ 0
DAC_1A
Text GLabel 10250 5525 2    50   Input ~ 0
DAC_1D
Text GLabel 9750 5225 0    50   Input ~ 0
DAC_2B
Text GLabel 9750 5325 0    50   Input ~ 0
DAC_2C
Text GLabel 9750 5425 0    50   Input ~ 0
DAC_1B
Text GLabel 9750 5525 0    50   Input ~ 0
DAC_1C
$Comp
L power:GND #PWR0139
U 1 1 61449ECF
P 9750 5625
F 0 "#PWR0139" H 9750 5375 50  0001 C CNN
F 1 "GND" H 9755 5452 50  0001 C CNN
F 2 "" H 9750 5625 50  0001 C CNN
F 3 "" H 9750 5625 50  0001 C CNN
	1    9750 5625
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0140
U 1 1 6144A49B
P 10250 5625
F 0 "#PWR0140" H 10250 5375 50  0001 C CNN
F 1 "GND" H 10255 5452 50  0001 C CNN
F 2 "" H 10250 5625 50  0001 C CNN
F 3 "" H 10250 5625 50  0001 C CNN
	1    10250 5625
	0    -1   -1   0   
$EndComp
Text GLabel 9750 5725 0    50   Input ~ 0
DAC_4B
Text GLabel 9750 5825 0    50   Input ~ 0
DAC_4C
Text GLabel 9750 5925 0    50   Input ~ 0
DAC_3B
Text GLabel 9750 6025 0    50   Input ~ 0
DAC_3C
Text GLabel 10250 5725 2    50   Input ~ 0
DAC_4A
Text GLabel 10250 5825 2    50   Input ~ 0
DAC_4D
Text GLabel 10250 5925 2    50   Input ~ 0
DAC_3A
Text GLabel 10250 6025 2    50   Input ~ 0
DAC_3D
$Comp
L power:GND #PWR0141
U 1 1 6144B341
P 9750 6125
F 0 "#PWR0141" H 9750 5875 50  0001 C CNN
F 1 "GND" H 9755 5952 50  0001 C CNN
F 2 "" H 9750 6125 50  0001 C CNN
F 3 "" H 9750 6125 50  0001 C CNN
	1    9750 6125
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0142
U 1 1 6144B82A
P 10250 6125
F 0 "#PWR0142" H 10250 5875 50  0001 C CNN
F 1 "GND" H 10255 5952 50  0001 C CNN
F 2 "" H 10250 6125 50  0001 C CNN
F 3 "" H 10250 6125 50  0001 C CNN
	1    10250 6125
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0143
U 1 1 6144D1D6
P 7050 5475
F 0 "#PWR0143" H 7050 5225 50  0001 C CNN
F 1 "GND" H 7055 5302 50  0001 C CNN
F 2 "" H 7050 5475 50  0001 C CNN
F 3 "" H 7050 5475 50  0001 C CNN
	1    7050 5475
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0144
U 1 1 6144D84D
P 8950 5475
F 0 "#PWR0144" H 8950 5225 50  0001 C CNN
F 1 "GND" H 8955 5302 50  0001 C CNN
F 2 "" H 8950 5475 50  0001 C CNN
F 3 "" H 8950 5475 50  0001 C CNN
	1    8950 5475
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 5475 8850 5475
Connection ~ 8950 5475
Connection ~ 7050 5475
Connection ~ 7150 5475
Wire Wire Line
	7150 5475 7050 5475
Connection ~ 7250 5475
Wire Wire Line
	7250 5475 7150 5475
Connection ~ 7350 5475
Wire Wire Line
	7350 5475 7250 5475
Connection ~ 7450 5475
Wire Wire Line
	7450 5475 7350 5475
Connection ~ 7550 5475
Wire Wire Line
	7550 5475 7450 5475
Connection ~ 7650 5475
Wire Wire Line
	7650 5475 7550 5475
Connection ~ 7750 5475
Wire Wire Line
	7750 5475 7650 5475
Connection ~ 7850 5475
Wire Wire Line
	7850 5475 7750 5475
Connection ~ 7950 5475
Wire Wire Line
	7950 5475 7850 5475
Connection ~ 8050 5475
Wire Wire Line
	8050 5475 7950 5475
Connection ~ 8150 5475
Wire Wire Line
	8150 5475 8050 5475
Connection ~ 8250 5475
Wire Wire Line
	8250 5475 8150 5475
Connection ~ 8350 5475
Wire Wire Line
	8350 5475 8250 5475
Connection ~ 8450 5475
Wire Wire Line
	8450 5475 8350 5475
Connection ~ 8550 5475
Wire Wire Line
	8550 5475 8450 5475
Connection ~ 8650 5475
Wire Wire Line
	8650 5475 8550 5475
Connection ~ 8750 5475
Wire Wire Line
	8750 5475 8650 5475
Connection ~ 8850 5475
Wire Wire Line
	8850 5475 8750 5475
$Comp
L power:GND #PWR0145
U 1 1 6144E74F
P 7050 4975
F 0 "#PWR0145" H 7050 4725 50  0001 C CNN
F 1 "GND" H 7055 4802 50  0001 C CNN
F 2 "" H 7050 4975 50  0001 C CNN
F 3 "" H 7050 4975 50  0001 C CNN
	1    7050 4975
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0146
U 1 1 6144ECCD
P 8950 4975
F 0 "#PWR0146" H 8950 4725 50  0001 C CNN
F 1 "GND" H 8955 4802 50  0001 C CNN
F 2 "" H 8950 4975 50  0001 C CNN
F 3 "" H 8950 4975 50  0001 C CNN
	1    8950 4975
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0147
U 1 1 6144F24A
P 8050 4975
F 0 "#PWR0147" H 8050 4725 50  0001 C CNN
F 1 "GND" H 8055 4802 50  0001 C CNN
F 2 "" H 8050 4975 50  0001 C CNN
F 3 "" H 8050 4975 50  0001 C CNN
	1    8050 4975
	-1   0    0    1   
$EndComp
Wire Wire Line
	7250 4975 7150 4975
Connection ~ 7050 4975
Connection ~ 7150 4975
Wire Wire Line
	7150 4975 7050 4975
NoConn ~ 5750 6025
NoConn ~ 6250 6025
Text Label 2350 2900 0    50   ~ 0
board_3v3
Text Label 1600 2900 2    50   ~ 0
board_3v3a
Text Label 2000 1150 1    50   ~ 0
board_3v3
Text GLabel 10600 1525 0    50   Input ~ 0
A2*
Text GLabel 10600 975  0    50   Input ~ 0
A1*
Text GLabel 10700 1525 2    50   Input ~ 0
OSC_A
Text GLabel 10700 975  2    50   Input ~ 0
OSC_B
Wire Wire Line
	10700 975  10650 975 
Wire Wire Line
	10650 800  10650 975 
Connection ~ 10650 975 
Wire Wire Line
	10650 975  10600 975 
Wire Wire Line
	10650 1350 10650 1525
Wire Wire Line
	10650 1525 10700 1525
Wire Wire Line
	10600 1525 10650 1525
Connection ~ 10650 1525
Text Label 7450 2275 2    50   ~ 0
pollux_ramp
Text Label 7450 2375 2    50   ~ 0
pollux_pwm
Text Label 7450 2475 2    50   ~ 0
pollux_sub
Text Label 7450 2775 2    50   ~ 0
castor_ramp
Text Label 7450 2875 2    50   ~ 0
castor_pwm
Text Label 7450 2975 2    50   ~ 0
castor_sub
Text Label 3925 950  0    50   ~ 0
pollux_ramp_mix_in
Text Label 3925 1300 0    50   ~ 0
pollux_pwm_mix_in
Text Label 7450 2575 2    50   ~ 0
pollux_mix_out
Text Label 7450 3075 2    50   ~ 0
castor_mix_out
Text Label 8875 2125 0    50   ~ 0
cv_a_pot
Text Label 8875 2725 0    50   ~ 0
cv_a_jack
Text Label 4300 3025 0    50   ~ 0
chorus_pot
Text Label 8875 2225 0    50   ~ 0
cv_b_pot
Text Label 4275 2625 0    50   ~ 0
duty_a_pot
Text Label 4275 2725 0    50   ~ 0
duty_b_pot
Text Label 8875 2825 0    50   ~ 0
cv_b_jack
Text GLabel 8825 2125 0    50   Input ~ 0
DAC_2A
Text GLabel 8825 2725 0    50   Input ~ 0
DAC_1A
$Comp
L power:GND #PWR0148
U 1 1 61466046
P 4300 3025
F 0 "#PWR0148" H 4300 2775 50  0001 C CNN
F 1 "GND" V 4305 2897 50  0001 R CNN
F 2 "" H 4300 3025 50  0001 C CNN
F 3 "" H 4300 3025 50  0001 C CNN
	1    4300 3025
	0    1    1    0   
$EndComp
Text GLabel 7525 2275 2    50   Input ~ 0
AUDIO_IN_2A
Text GLabel 7525 2375 2    50   Input ~ 0
AUDIO_IN_2B
Text GLabel 7525 2475 2    50   Input ~ 0
AUDIO_IN_2C
Text GLabel 7525 2775 2    50   Input ~ 0
AUDIO_IN_1A
Text GLabel 7525 2875 2    50   Input ~ 0
AUDIO_IN_1B
Text GLabel 7525 2975 2    50   Input ~ 0
AUDIO_IN_1C
Text GLabel 7525 2575 2    50   Input ~ 0
AUDIO_IN_2D
Text GLabel 7525 3075 2    50   Input ~ 0
AUDIO_IN_1D
$Comp
L Jumper:Jumper_2_Open JP1
U 1 1 614689B3
P 3725 950
F 0 "JP1" H 3725 1185 50  0000 C CNN
F 1 "P Ramp Mix" H 3725 1094 50  0000 C CNN
F 2 "TestPoint:TestPoint_2Pads_Pitch2.54mm_Drill0.8mm" H 3725 950 50  0001 C CNN
F 3 "~" H 3725 950 50  0001 C CNN
	1    3725 950 
	1    0    0    -1  
$EndComp
Text Label 3925 1625 0    50   ~ 0
pollux_sub_mix_in
Text Label 3525 950  2    50   ~ 0
pollux_ramp
Text Label 3525 1300 2    50   ~ 0
pollux_pwm
Text Label 3525 1625 2    50   ~ 0
pollux_sub
$Comp
L Jumper:Jumper_2_Open JP2
U 1 1 61469577
P 3725 1300
F 0 "JP2" H 3725 1535 50  0000 C CNN
F 1 "P Pulse Mix" H 3725 1444 50  0000 C CNN
F 2 "TestPoint:TestPoint_2Pads_Pitch2.54mm_Drill0.8mm" H 3725 1300 50  0001 C CNN
F 3 "~" H 3725 1300 50  0001 C CNN
	1    3725 1300
	1    0    0    -1  
$EndComp
$Comp
L Jumper:Jumper_2_Open JP3
U 1 1 6146A7F2
P 3725 1625
F 0 "JP3" H 3725 1860 50  0000 C CNN
F 1 "P Sub Mix" H 3725 1769 50  0000 C CNN
F 2 "TestPoint:TestPoint_2Pads_Pitch2.54mm_Drill0.8mm" H 3725 1625 50  0001 C CNN
F 3 "~" H 3725 1625 50  0001 C CNN
	1    3725 1625
	1    0    0    -1  
$EndComp
Text Label 5825 950  0    50   ~ 0
castor_ramp_mix_in
Text Label 5825 1300 0    50   ~ 0
castor_pwm_mix_in
$Comp
L Jumper:Jumper_2_Open JP4
U 1 1 6146E4AA
P 5625 950
F 0 "JP4" H 5625 1185 50  0000 C CNN
F 1 "C Ramp Mix" H 5625 1094 50  0000 C CNN
F 2 "TestPoint:TestPoint_2Pads_Pitch2.54mm_Drill0.8mm" H 5625 950 50  0001 C CNN
F 3 "~" H 5625 950 50  0001 C CNN
	1    5625 950 
	1    0    0    -1  
$EndComp
Text Label 5825 1625 0    50   ~ 0
castor_sub_mix_in
Text Label 5425 950  2    50   ~ 0
castor_ramp
Text Label 5425 1300 2    50   ~ 0
castor_pwm
Text Label 5425 1625 2    50   ~ 0
castor_sub
$Comp
L Jumper:Jumper_2_Open JP5
U 1 1 6146E4B4
P 5625 1300
F 0 "JP5" H 5625 1535 50  0000 C CNN
F 1 "C Pulse Mix" H 5625 1444 50  0000 C CNN
F 2 "TestPoint:TestPoint_2Pads_Pitch2.54mm_Drill0.8mm" H 5625 1300 50  0001 C CNN
F 3 "~" H 5625 1300 50  0001 C CNN
	1    5625 1300
	1    0    0    -1  
$EndComp
$Comp
L Jumper:Jumper_2_Open JP6
U 1 1 6146E4BA
P 5625 1625
F 0 "JP6" H 5625 1860 50  0000 C CNN
F 1 "C Sub Mix" H 5625 1769 50  0000 C CNN
F 2 "TestPoint:TestPoint_2Pads_Pitch2.54mm_Drill0.8mm" H 5625 1625 50  0001 C CNN
F 3 "~" H 5625 1625 50  0001 C CNN
	1    5625 1625
	1    0    0    -1  
$EndComp
Text GLabel 8825 2225 0    50   Input ~ 0
DAC_2B
Text GLabel 8825 2825 0    50   Input ~ 0
DAC_1B
Text Label 4275 2625 2    50   ~ 0
board_3v3a
Text Label 4275 2725 2    50   ~ 0
board_3v3a
$Comp
L Mechanical:MountingHole_Pad H6
U 1 1 6148579C
P 4325 6450
F 0 "H6" H 4225 6407 50  0000 R CNN
F 1 "M3" H 4225 6498 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4325 6450 50  0001 C CNN
F 3 "~" H 4325 6450 50  0001 C CNN
	1    4325 6450
	-1   0    0    1   
$EndComp
NoConn ~ 4325 6350
$Comp
L Mechanical:MountingHole_Pad H7
U 1 1 614857A7
P 4325 6775
F 0 "H7" H 4225 6732 50  0000 R CNN
F 1 "M3" H 4225 6823 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4325 6775 50  0001 C CNN
F 3 "~" H 4325 6775 50  0001 C CNN
	1    4325 6775
	-1   0    0    1   
$EndComp
NoConn ~ 4325 6675
$Comp
L Mechanical:MountingHole_Pad H8
U 1 1 614857B2
P 4325 7075
F 0 "H8" H 4225 7032 50  0000 R CNN
F 1 "M3" H 4225 7123 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4325 7075 50  0001 C CNN
F 3 "~" H 4325 7075 50  0001 C CNN
	1    4325 7075
	-1   0    0    1   
$EndComp
NoConn ~ 4325 6975
$Comp
L Mechanical:MountingHole_Pad H9
U 1 1 614857BD
P 4325 7350
F 0 "H9" H 4225 7307 50  0000 R CNN
F 1 "M3" H 4225 7398 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4325 7350 50  0001 C CNN
F 3 "~" H 4325 7350 50  0001 C CNN
	1    4325 7350
	-1   0    0    1   
$EndComp
NoConn ~ 4325 7250
Text GLabel 8150 4975 1    50   Input ~ 0
SERIAL1.0
Text GLabel 8250 4975 1    50   Input ~ 0
SERIAL1.1
Text GLabel 8350 4975 1    50   Input ~ 0
SERIAL1.2
Text GLabel 8450 4975 1    50   Input ~ 0
SERIAL1.3
Text GLabel 8825 3250 2    50   Input ~ 0
SERIAL1.0
Text GLabel 8825 3350 2    50   Input ~ 0
SERIAL1.1
Text GLabel 8825 3450 2    50   Input ~ 0
SERIAL1.2
Text GLabel 8825 3550 2    50   Input ~ 0
SERIAL1.3
Text Label 1150 6350 1    50   ~ 0
board_3v3
Wire Wire Line
	7525 2275 7450 2275
Wire Wire Line
	7450 2375 7525 2375
Wire Wire Line
	7525 2475 7450 2475
Wire Wire Line
	7450 2575 7525 2575
Wire Wire Line
	7450 2775 7525 2775
Wire Wire Line
	7525 2875 7450 2875
Wire Wire Line
	7450 2975 7525 2975
Wire Wire Line
	7525 3075 7450 3075
Wire Wire Line
	8825 2125 8875 2125
Wire Wire Line
	8875 2225 8825 2225
Wire Wire Line
	8825 2725 8875 2725
Wire Wire Line
	8875 2825 8825 2825
$EndSCHEMATC
