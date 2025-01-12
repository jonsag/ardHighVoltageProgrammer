#include <Arduino.h>

// User defined settings
#define  MEGA         0       // Set this to 1 if you are using an Arduino Mega (default = 0)
#define  DEFAULTMODE  ATMEGA  // If running in non-interactive mode, you need to set this to ATMEGA, TINY2313, or HVSP.
#define  ASKMODE      1       // Set this to 1 to enable mode question at startup
#define  INTERACTIVE  1       // Set this to 0 to disable interactive (serial) mode
#define  BURN_EFUSE   0       // Set this to 1 to enable burning extended fuse byte
#define  BAUD         9600    // Serial port rate at which to talk to PC

// If interactive mode is off, these fuse settings are used instead of user prompted values
#define  LFUSE        0x62  // default for ATmega168 = 0x62
#define  HFUSE        0xDF  // default for ATmega168 = 0xDF
#define  EFUSE        0xF9  // default for ATmega168 = 0xF9

/*
  Data line assignments
  Fuse and command data for HVPP mode are sent using Arduino digital lines 0-7
  
  Arduino Uno and other original-style form factor boards:
    Digital Line 0-7 outputs = PORTD
    Inputs = PIND
    Data direction register = DDRD

  Arduino Mega - much more complicated because digital lines 0-7 don't map directly to an AVR hardware port:
    Digital Line  AVR Signal Name
    0  PE0  (PORTE)
    1  PE1  (PORTE)  
    2  PE4  (PORTE)
    3  PE5  (PORTE)
    4  PG5  (PORTG)
    5  PE3  (PORTE) 
    6  PH3  (PORTH)
    7  PH4  (PORTH)
*/

// Pin Assignments (you shouldn't need to change these)
#define  VCC     12
#define  RDY     13     // RDY/!BSY signal from target
#define  OE      11
#define  WR      10
#define  BS1     A2
#define  XA0     8
#define  XA1     A4    
#define  RST     A0    // 12V Step up converter enable (12V_EN)
#define  XTAL1   A3
#define  BUTTON  A1    // Run button

// Pin assignments for HVSP mode 
#define  SCI    BS1
#define  SDO    RDY
#define  SII    XA0
#define  SDI    XA1

// Serial instructions for HVSP mode
// Based on the ATtiny85 datasheet Table 20-16 pg. 163-165.
// These instructions don't contain the necessary zero padding, which is added later.

// LFUSE
#define HVSP_READ_LFUSE_DATA     B00000100  // For the commands we are interested in
#define HVSP_READ_LFUSE_INSTR1   B01001100  // only the 1st instruction contains a fixed data packet.
#define HVSP_READ_LFUSE_INSTR2   B01101000  // Instructions 2-3 have data = all zeros.
#define HVSP_READ_LFUSE_INSTR3   B01101100

#define HVSP_WRITE_LFUSE_DATA    B01000000  // For the write instructions, the data contents
#define HVSP_WRITE_LFUSE_INSTR1  B01001100  // for instruction 2 are the desired fuse bits.
#define HVSP_WRITE_LFUSE_INSTR2  B00101100  // Instructions 3-4 have data = all zeros.
#define HVSP_WRITE_LFUSE_INSTR3  B01100100
#define HVSP_WRITE_LFUSE_INSTR4  B01101100

// HFUSE
#define HVSP_READ_HFUSE_DATA     B00000100
#define HVSP_READ_HFUSE_INSTR1   B01001100
#define HVSP_READ_HFUSE_INSTR2   B01111010
#define HVSP_READ_HFUSE_INSTR3   B01111110

#define HVSP_WRITE_HFUSE_DATA    B01000000
#define HVSP_WRITE_HFUSE_INSTR1  B01001100
#define HVSP_WRITE_HFUSE_INSTR2  B00101100
#define HVSP_WRITE_HFUSE_INSTR3  B01110100
#define HVSP_WRITE_HFUSE_INSTR4  B01111100

// EFUSE
// Note: not all ATtiny's have an EFUSE
#define HVSP_READ_EFUSE_DATA     B00000100  
#define HVSP_READ_EFUSE_INSTR1   B01001100
#define HVSP_READ_EFUSE_INSTR2   B01101010
#define HVSP_READ_EFUSE_INSTR3   B01101110

#define HVSP_WRITE_EFUSE_DATA    B01000000
#define HVSP_WRITE_EFUSE_INSTR1  B01001100
#define HVSP_WRITE_EFUSE_INSTR2  B00101100
#define HVSP_WRITE_EFUSE_INSTR3  B01100110
#define HVSP_WRITE_EFUSE_INSTR4  B01101110

// Enable debug mode by uncommenting this line
//#define DEBUG

// Internal definitions
enum modelist { ATMEGA, TINY2313, HVSP };
enum fusesel { LFUSE_SEL, HFUSE_SEL, EFUSE_SEL };

// Global variables
byte mode = DEFAULTMODE;  // programming mode

// These pin assignments change depending on which chip is being programmed,
// so they can't be set using #define
// There is probably a more elegant way to do this.  Suggestions?
byte PAGEL = A5;  // ATtiny2313: PAGEL = BS1
byte BS2 = 9;     // ATtiny2313: BS2 = XA1
