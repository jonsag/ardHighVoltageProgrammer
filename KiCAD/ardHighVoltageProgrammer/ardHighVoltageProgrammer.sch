EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L My_Arduino:Arduino_Mini_Pro U2
U 1 1 614B3BA2
P 8100 4100
F 0 "U2" H 8850 4581 50  0000 C CNN
F 1 "Arduino_Mini_Pro" H 8850 4490 50  0000 C CNN
F 2 "My_Arduino:Arduino_Pro_Mini_vC_larger_pads" H 8900 2200 50  0001 C CNN
F 3 "" H 8350 4250 50  0001 C CNN
	1    8100 4100
	1    0    0    -1  
$EndComp
$Comp
L My_Misc:ATmega328P-PU U1
U 1 1 614B4341
P 2800 3100
F 0 "U1" H 2156 3146 50  0000 R CNN
F 1 "ATmega328P-PU" H 2156 3055 50  0000 R CNN
F 2 "Package_DIP:DIP-28_W7.62mm_Socket_LongPads" H 2800 3100 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 2800 3100 50  0001 C CNN
	1    2800 3100
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N3904 Q1
U 1 1 614B63B5
P 9350 2100
F 0 "Q1" H 9540 2146 50  0000 L CNN
F 1 "2N3904" H 9540 2055 50  0000 L CNN
F 2 "My_Parts:TO-220F-3_Vertical_large" H 9550 2025 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/2N3903-D.PDF" H 9350 2100 50  0001 L CNN
	1    9350 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 614B6815
P 7800 2550
F 0 "D1" H 7793 2767 50  0000 C CNN
F 1 "LED" H 7793 2676 50  0000 C CNN
F 2 "My_Misc:LED_D5.0mm_larger_pads" H 7800 2550 50  0001 C CNN
F 3 "~" H 7800 2550 50  0001 C CNN
	1    7800 2550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 614B76A8
P 8800 1650
F 0 "R2" H 8870 1696 50  0000 L CNN
F 1 "1k" H 8870 1605 50  0000 L CNN
F 2 "My_Misc:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal_larger_pads" V 8730 1650 50  0001 C CNN
F 3 "~" H 8800 1650 50  0001 C CNN
	1    8800 1650
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 614B7923
P 8300 2550
F 0 "R1" H 8370 2596 50  0000 L CNN
F 1 "390R" H 8370 2505 50  0000 L CNN
F 2 "My_Misc:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal_larger_pads" V 8230 2550 50  0001 C CNN
F 3 "~" H 8300 2550 50  0001 C CNN
	1    8300 2550
	0    -1   -1   0   
$EndComp
Text GLabel 9700 4700 2    50   Input ~ 0
DATA0
Text GLabel 8000 4100 0    50   Input ~ 0
DATA1
Text GLabel 8000 4500 0    50   Input ~ 0
DATA2
Text GLabel 8000 4600 0    50   Input ~ 0
DATA3
Text GLabel 8000 4700 0    50   Input ~ 0
DATA4
Text GLabel 8000 4800 0    50   Input ~ 0
DATA5
Text GLabel 8000 4900 0    50   Input ~ 0
DATA6
Text GLabel 8000 5000 0    50   Input ~ 0
DATA7
Text GLabel 8000 5100 0    50   Input ~ 0
XA0
Text GLabel 8000 5200 0    50   Input ~ 0
BS2
Text GLabel 9700 5200 2    50   Input ~ 0
WR
Text GLabel 9700 5100 2    50   Input ~ 0
OE
Text GLabel 9700 5000 2    50   Input ~ 0
VCC
Text GLabel 9700 4900 2    50   Input ~ 0
RDY
Text GLabel 9700 4800 2    50   Input ~ 0
RST
Text GLabel 9700 4600 2    50   Input ~ 0
BS1
Text GLabel 9700 4500 2    50   Input ~ 0
XTAL1
Text GLabel 8800 5600 3    50   Input ~ 0
XA1
Text GLabel 8900 5600 3    50   Input ~ 0
PAGEL
Wire Wire Line
	9600 4500 9700 4500
Wire Wire Line
	9600 4600 9700 4600
Wire Wire Line
	9700 4700 9600 4700
Wire Wire Line
	9600 4800 9700 4800
Wire Wire Line
	9600 4900 9700 4900
Wire Wire Line
	9600 5000 9700 5000
Wire Wire Line
	9600 5100 9700 5100
Wire Wire Line
	8900 5500 8900 5600
Wire Wire Line
	8800 5500 8800 5600
Wire Wire Line
	8000 5200 8100 5200
Wire Wire Line
	8000 5100 8100 5100
Wire Wire Line
	8100 4500 8000 4500
Wire Wire Line
	8000 4600 8100 4600
Wire Wire Line
	8000 4700 8100 4700
Wire Wire Line
	8000 4800 8100 4800
Wire Wire Line
	8000 4900 8100 4900
Wire Wire Line
	8000 5000 8100 5000
Wire Wire Line
	9600 5200 9700 5200
Wire Wire Line
	8000 4100 8100 4100
$Comp
L Device:R R3
U 1 1 614B6F70
P 8800 2100
F 0 "R3" H 8870 2146 50  0000 L CNN
F 1 "1k" H 8870 2055 50  0000 L CNN
F 2 "My_Misc:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal_larger_pads" V 8730 2100 50  0001 C CNN
F 3 "~" H 8800 2100 50  0001 C CNN
	1    8800 2100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8950 2100 9150 2100
Wire Wire Line
	8950 1650 9450 1650
Wire Wire Line
	9450 1650 9450 1900
Wire Wire Line
	7950 2550 8150 2550
Wire Wire Line
	8450 2550 8550 2550
Wire Wire Line
	8550 2550 8550 2100
Wire Wire Line
	8550 2100 8650 2100
$Comp
L power:+5V #PWR0101
U 1 1 614D81CD
P 10000 3650
F 0 "#PWR0101" H 10000 3500 50  0001 C CNN
F 1 "+5V" H 10015 3823 50  0000 C CNN
F 2 "" H 10000 3650 50  0001 C CNN
F 3 "" H 10000 3650 50  0001 C CNN
	1    10000 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 4400 10000 4400
Wire Wire Line
	10000 4400 10000 3650
Wire Wire Line
	9600 4200 10300 4200
Wire Wire Line
	10300 4200 10300 5550
$Comp
L power:GND #PWR0102
U 1 1 614D978B
P 10300 5550
F 0 "#PWR0102" H 10300 5300 50  0001 C CNN
F 1 "GND" H 10305 5377 50  0000 C CNN
F 2 "" H 10300 5550 50  0001 C CNN
F 3 "" H 10300 5550 50  0001 C CNN
	1    10300 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 614D9A6D
P 9450 2550
F 0 "#PWR0103" H 9450 2300 50  0001 C CNN
F 1 "GND" H 9455 2377 50  0000 C CNN
F 2 "" H 9450 2550 50  0001 C CNN
F 3 "" H 9450 2550 50  0001 C CNN
	1    9450 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 2550 9450 2300
Wire Wire Line
	7650 2550 7450 2550
Wire Wire Line
	7450 2550 7450 2150
$Comp
L power:+5V #PWR0104
U 1 1 614DC709
P 7450 2150
F 0 "#PWR0104" H 7450 2000 50  0001 C CNN
F 1 "+5V" H 7465 2323 50  0000 C CNN
F 2 "" H 7450 2150 50  0001 C CNN
F 3 "" H 7450 2150 50  0001 C CNN
	1    7450 2150
	1    0    0    -1  
$EndComp
Text GLabel 8750 2550 2    50   Input ~ 0
RST
Wire Wire Line
	8750 2550 8550 2550
Connection ~ 8550 2550
$Comp
L My_Headers:2-pin_power_input_header J1
U 1 1 614DE700
P 6750 1650
F 0 "J1" H 6713 1783 50  0000 C CNN
F 1 "2-pin_power_input_header" H 6750 1450 50  0001 C CNN
F 2 "My_Headers:2-pin_power_input_header_larger_pads" H 6800 1350 50  0001 C CNN
F 3 "~" H 6750 1650 50  0001 C CNN
	1    6750 1650
	-1   0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0105
U 1 1 614DFD8B
P 7150 1350
F 0 "#PWR0105" H 7150 1200 50  0001 C CNN
F 1 "+12V" H 7165 1523 50  0000 C CNN
F 2 "" H 7150 1350 50  0001 C CNN
F 3 "" H 7150 1350 50  0001 C CNN
	1    7150 1350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 614E0288
P 7150 2850
F 0 "#PWR0106" H 7150 2600 50  0001 C CNN
F 1 "GND" H 7155 2677 50  0000 C CNN
F 2 "" H 7150 2850 50  0001 C CNN
F 3 "" H 7150 2850 50  0001 C CNN
	1    7150 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 1650 7150 1650
Wire Wire Line
	7150 1650 7150 1350
Wire Wire Line
	6950 1750 7150 1750
Wire Wire Line
	7150 1750 7150 2850
Connection ~ 7150 1650
Wire Wire Line
	7150 1650 8650 1650
$Comp
L power:GND #PWR0107
U 1 1 614F023C
P 2800 4900
F 0 "#PWR0107" H 2800 4650 50  0001 C CNN
F 1 "GND" H 2805 4727 50  0000 C CNN
F 2 "" H 2800 4900 50  0001 C CNN
F 3 "" H 2800 4900 50  0001 C CNN
	1    2800 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 4600 2800 4700
Wire Wire Line
	2800 4700 2900 4700
Wire Wire Line
	2900 4700 2900 4600
Connection ~ 2800 4700
Wire Wire Line
	2800 4700 2800 4900
$Comp
L power:+5V #PWR0108
U 1 1 614F34EC
P 2800 1300
F 0 "#PWR0108" H 2800 1150 50  0001 C CNN
F 1 "+5V" H 2815 1473 50  0000 C CNN
F 2 "" H 2800 1300 50  0001 C CNN
F 3 "" H 2800 1300 50  0001 C CNN
	1    2800 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 1300 2800 1500
Wire Wire Line
	2900 1600 2900 1500
Wire Wire Line
	2900 1500 2800 1500
Connection ~ 2800 1500
Wire Wire Line
	2800 1500 2800 1600
Text GLabel 3500 2500 2    50   Input ~ 0
XTAL1
Text GLabel 3500 3400 2    50   Input ~ 0
RESET
Text GLabel 3500 2400 2    50   Input ~ 0
DATA5
Text GLabel 3500 2300 2    50   Input ~ 0
DATA4
Text GLabel 3500 2200 2    50   Input ~ 0
DATA3
Text GLabel 3500 2100 2    50   Input ~ 0
DATA2
Text GLabel 3500 2000 2    50   Input ~ 0
DATA1
Text GLabel 3500 1900 2    50   Input ~ 0
DATA0
Text GLabel 3500 4300 2    50   Input ~ 0
PAGEL
Text GLabel 3500 4200 2    50   Input ~ 0
XA1
Text GLabel 3500 4100 2    50   Input ~ 0
XA0
Text GLabel 3500 4000 2    50   Input ~ 0
BS1
Text GLabel 3500 3900 2    50   Input ~ 0
WR
Text GLabel 3500 3800 2    50   Input ~ 0
OE
Text GLabel 3500 3700 2    50   Input ~ 0
RDY
Text GLabel 3500 3000 2    50   Input ~ 0
BS2
Text GLabel 3500 2900 2    50   Input ~ 0
DATA7
Text GLabel 3500 2800 2    50   Input ~ 0
DATA6
Wire Wire Line
	3400 1900 3500 1900
Wire Wire Line
	3500 2000 3400 2000
Wire Wire Line
	3400 2100 3500 2100
Wire Wire Line
	3500 2200 3400 2200
Wire Wire Line
	3400 2300 3500 2300
Wire Wire Line
	3500 2400 3400 2400
Wire Wire Line
	3400 2500 3500 2500
Wire Wire Line
	3500 2800 3400 2800
Wire Wire Line
	3400 2900 3500 2900
Wire Wire Line
	3500 3000 3400 3000
Wire Wire Line
	3400 3400 3500 3400
Wire Wire Line
	3500 3700 3400 3700
Wire Wire Line
	3400 3800 3500 3800
Wire Wire Line
	3500 3900 3400 3900
Wire Wire Line
	3400 4000 3500 4000
Wire Wire Line
	3500 4100 3400 4100
Wire Wire Line
	3400 4200 3500 4200
Wire Wire Line
	3500 4300 3400 4300
$EndSCHEMATC
