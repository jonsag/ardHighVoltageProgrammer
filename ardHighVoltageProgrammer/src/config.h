#include <Arduino.h>

// User defined settings
#define DEFAULTMODE ATMEGA // If running in non-interactive mode, you need to set this to ATMEGA, TINY2313, or HVSP.
#define INTERACTIVE 1      // Set this to 0 to disable interactive mode
#define BURN_EFUSE 1       // Set this to 1 to enable burning extended fuse byte
#define BAUD 115200        // Serial port rate at which to talk to PC
#define SERIALTIMEOUT 3000 // Amount of time to wait for complete input if CR/LF off

// If interactive mode is off, these fuse settings are used instead of user prompted values
#define LFUSE 0x62 // default for ATmega328P per datasheet p282-284 0x62 UNO=0xFF
#define HFUSE 0xD9 // default for ATmega328P per datasheet p282-284 0xD9 UNO=0xDE Duemilanove328=0xDA
#define EFUSE 0xFF // default for ATmega328P per datasheet p282-284 0xFF UNO=0x05(effective FD)
// default low fuse  for ATtiny85 per datasheet p148-149 0x62 digispark=0xF1
// default high fuse for ATtiny85 per datasheet p148-149 0xDF digispark=0xD5
// default ext fuse  for ATtiny85 per datasheet p148-149 0xFF digispark=0xFE
// default low fuse  for ATtiny2313 per datasheet p159-160 0x64
// default high fuse for ATtiny2313 per datasheet p159-160 0xDF
// default ext fuse  for ATtiny2313 per datasheet p159-160 0xFF

// Pin Assignments
#define DATA0 A1
#define DATA1 1
#define DATA2 2
#define DATA3 3
#define DATA4 4
#define DATA5 5
#define DATA6 6
#define DATA7 7
#define VCC 12
#define RDY 13 // RDY/!BSY signal from target
#define OE 11
#define WR 10
#define BS1 A2
#define XA0 8
#define XA1 A4
#define RST A0 // 12V to target reset enable (!12V_EN)
#define XTAL1 A3
#define PAGELM A5
#define BS2M 9

// Pin assignment adjustments for Tiny
#define PAGELT BS1 // PAGEL is set to same pin as BS1 for ATtiny2313
#define BS2T XA1   // BS2 is set to same pin as XA1 for ATtiny2313
// Pin assignments for HVSP mode
#define SCI BS1
#define SDO RDY
#define SII XA0
#define SDI XA1

// Serial instructions for HVSP mode
// Based on the ATtiny85 datasheet Table 20-16 pg. 163-165.
// These instructions don't contain the necessary zero padding, which is added later.

// LFUSE
#define HVSP_READ_LFUSE_DATA B00000100   // For the commands we are interested in
#define HVSP_READ_LFUSE_INSTR1 B01001100 // only the 1st instruction contains a fixed data packet.
#define HVSP_READ_LFUSE_INSTR2 B01101000 // Instructions 2-3 have data = all zeros.
#define HVSP_READ_LFUSE_INSTR3 B01101100

#define HVSP_WRITE_LFUSE_DATA B01000000   // For the write instructions, the data contents
#define HVSP_WRITE_LFUSE_INSTR1 B01001100 // for instruction 2 are the desired fuse bits.
#define HVSP_WRITE_LFUSE_INSTR2 B00101100 // Instructions 3-4 have data = all zeros.
#define HVSP_WRITE_LFUSE_INSTR3 B01100100
#define HVSP_WRITE_LFUSE_INSTR4 B01101100

// HFUSE
#define HVSP_READ_HFUSE_DATA B00000100
#define HVSP_READ_HFUSE_INSTR1 B01001100
#define HVSP_READ_HFUSE_INSTR2 B01111010
#define HVSP_READ_HFUSE_INSTR3 B01111110

#define HVSP_WRITE_HFUSE_DATA B01000000
#define HVSP_WRITE_HFUSE_INSTR1 B01001100
#define HVSP_WRITE_HFUSE_INSTR2 B00101100
#define HVSP_WRITE_HFUSE_INSTR3 B01110100
#define HVSP_WRITE_HFUSE_INSTR4 B01111100

// EFUSE
// Note: not all ATtiny's have an EFUSE
#define HVSP_READ_EFUSE_DATA B00000100
#define HVSP_READ_EFUSE_INSTR1 B01001100
#define HVSP_READ_EFUSE_INSTR2 B01101010
#define HVSP_READ_EFUSE_INSTR3 B01101110

#define HVSP_WRITE_EFUSE_DATA B01000000
#define HVSP_WRITE_EFUSE_INSTR1 B01001100
#define HVSP_WRITE_EFUSE_INSTR2 B00101100
#define HVSP_WRITE_EFUSE_INSTR3 B01100110
#define HVSP_WRITE_EFUSE_INSTR4 B01101110

// Internal definitions
enum modelist
{
    ATMEGA,
    TINY2313,
    HVSP
};
enum fusesel
{
    LFUSE_SEL,
    HFUSE_SEL,
    EFUSE_SEL
};

// Global variables
byte mode = DEFAULTMODE;        // programming mode
String inputString = "";        // a string to hold incoming data
boolean stringComplete = false; // whether the string is complete
long val1;
byte base = 0;
byte lfuse = LFUSE;
byte hfuse = HFUSE;
byte efuse = EFUSE;

// These pin assignments change depending on which chip is being programmed,
// so they are put into variables
byte PAGEL = PAGELM;
byte BS2 = BS2M;