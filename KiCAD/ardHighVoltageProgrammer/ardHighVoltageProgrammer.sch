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
L My_Arduino:Arduino_Mini_Pro U?
U 1 1 614B3BA2
P 6950 2750
F 0 "U?" H 7700 3231 50  0000 C CNN
F 1 "Arduino_Mini_Pro" H 7700 3140 50  0000 C CNN
F 2 "" H 7750 850 50  0001 C CNN
F 3 "" H 7200 2900 50  0001 C CNN
	1    6950 2750
	1    0    0    -1  
$EndComp
$Comp
L My_Misc:ATmega328P-PU U?
U 1 1 614B4341
P 2800 3100
F 0 "U?" H 2156 3146 50  0000 R CNN
F 1 "ATmega328P-PU" H 2156 3055 50  0000 R CNN
F 2 "Package_DIP:DIP-28_W7.62mm" H 2800 3100 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 2800 3100 50  0001 C CNN
	1    2800 3100
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N3904 Q?
U 1 1 614B63B5
P 5000 5400
F 0 "Q?" H 5190 5446 50  0000 L CNN
F 1 "2N3904" H 5190 5355 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5200 5325 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/2N3903-D.PDF" H 5000 5400 50  0001 L CNN
	1    5000 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D?
U 1 1 614B6815
P 6700 5350
F 0 "D?" H 6693 5567 50  0000 C CNN
F 1 "LED" H 6693 5476 50  0000 C CNN
F 2 "" H 6700 5350 50  0001 C CNN
F 3 "~" H 6700 5350 50  0001 C CNN
	1    6700 5350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 614B6F70
P 8050 5050
F 0 "R?" H 8120 5096 50  0000 L CNN
F 1 "1k" H 8120 5005 50  0000 L CNN
F 2 "" V 7980 5050 50  0001 C CNN
F 3 "~" H 8050 5050 50  0001 C CNN
	1    8050 5050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 614B76A8
P 8700 5000
F 0 "R?" H 8770 5046 50  0000 L CNN
F 1 "1k" H 8770 4955 50  0000 L CNN
F 2 "" V 8630 5000 50  0001 C CNN
F 3 "~" H 8700 5000 50  0001 C CNN
	1    8700 5000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 614B7923
P 9500 5000
F 0 "R?" H 9570 5046 50  0000 L CNN
F 1 "390R" H 9570 4955 50  0000 L CNN
F 2 "" V 9430 5000 50  0001 C CNN
F 3 "~" H 9500 5000 50  0001 C CNN
	1    9500 5000
	1    0    0    -1  
$EndComp
$EndSCHEMATC
