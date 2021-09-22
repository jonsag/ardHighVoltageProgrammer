#include "config.h"
#include "modules.h"
/*
HVRescue Simple 2015-09-17 suitable for breadboard
instructables.com/id/HV-Rescue-Simple
Modified from HVRescue_Shield Arduino sketch 3/15/11 2.12 for use with the HV Rescue Shield
by Jeff Keyzer  mightyohm.com/hvrescue2

Enhancements/mods by dmjlambert
HV control inverted to work with 12V battery switched
  with a transistor as in the older Jeff Keyzer sketch
  and common HV programmer sketches for ATtiny.
Changed baud rate to 115200
Moved data0 from D0 to A1 to keep from conflicting with serial port
  and to work better with boards such as CH340G Nano
Changed direct port manipulation of portD to indivial Arduino pins
Changed default fuse values to ATmega328P typical values
Removed run button
Removed ASKMODE
Briefly go into programming mode just for the task at hand
Accept a variety of number formats, hex, decimal, binary, upper/lower case
Made tolerant of any line ending for serial input or no line ending at all
Made compatible with most Arduinos such as Uno, Leonardo, Mega2560, 
  Pro Mini, Nano
Consolidated/simplified/organized into sections
Incorporated new user interface and menu with these commands:
1 2 3 set mode
L set low fuse value
H set high fuse value
X set ext fuse value
B or U set typical L H and X fuse values to factory blank 328 or Uno
W write fuses
R read fuses
? or any invalid character display info
Attempted to preserve non-interactive setting.  Although the sketch
ends up at an interactive menu prompt, the work it does before getting
to the interactive state can be used.  Plug in a target chip and press
the Arduino reset button to program or read the chip fuses, then unplug
the chip and plug in another and reset to repeat.

The HVPP routines are based on those described in the ATmega48/88/168 datasheet rev.
2545M-AVR-09/07, pg. 290-297 and the ATtiny2313 datasheet rev. 2543I-AVR-04/06 pg. 165-176.

The HVSP routines are based on the ATtiny25/45/85 and 13A datasheets (ATtiny25/45/85 2586M–AVR–07/10 pg. 159-165,
ATtiny13A 8126E-AVR-7/10 pg. 109-116).

GNU General Public License gnu.org/licenses
*/

void setup()
{ // run once, when the sketch starts
  // reserve bytes for the inputString
  inputString.reserve(26);

  // Set up control lines for HV parallel programming

  // clear digital pins 0-7
  digitalWrite(DATA0, 0);
  digitalWrite(DATA1, 0);
  digitalWrite(DATA2, 0);
  digitalWrite(DATA3, 0);
  digitalWrite(DATA4, 0);
  digitalWrite(DATA5, 0);
  digitalWrite(DATA6, 0);
  digitalWrite(DATA7, 0);

  //  set digital pins 0-7 as inputs for now
  pinMode(DATA0, INPUT);
  pinMode(DATA1, INPUT);
  pinMode(DATA2, INPUT);
  pinMode(DATA3, INPUT);
  pinMode(DATA4, INPUT);
  pinMode(DATA5, INPUT);
  pinMode(DATA6, INPUT);
  pinMode(DATA7, INPUT);

  pinMode(VCC, OUTPUT);
  pinMode(RDY, INPUT);
  pinMode(OE, OUTPUT);
  pinMode(WR, OUTPUT);
  pinMode(BS1, OUTPUT);
  pinMode(XA0, OUTPUT);
  pinMode(XA1, OUTPUT);
  pinMode(PAGEL, OUTPUT);
  pinMode(RST, OUTPUT); // enable signal for DC-DC converter that generates +12V !RESET
  pinMode(BS2, OUTPUT);
  pinMode(XTAL1, OUTPUT);

  // Initialize output pins as needed
  digitalWrite(RST, HIGH); // Turn off 12V step-up converter (non-inverting, unlike original circuit)
  digitalWrite(VCC, LOW);

  Serial.begin(BAUD); // Open serial port
  delay(50);
#if defined __AVR_ATmega32U4__ && INTERACTIVE == 1
  // if Leonardo or similar wait for serial monitor connection
  while (!Serial)
    ;
#endif
  Serial.println(F("\n\nReset\n"));
  Serial.println(F(__FILE__));
  Serial.println();

} // end of setup

void loop()
{ // run over and over again

  byte read_hfuse, read_lfuse, read_efuse; // fuses read from target for verify
  // variable to store what to do next, options
  // 0=display choices
  // 1=prompt for a command
  // 2=do nothing and expect further input
  // 3=read fuses from target AVR
  // 4=write fuses to target AVR
  static int doNext = 0;
#if (INTERACTIVE == 0)
  if (doNext == 0)
  {
    doNext = 4; //non-interactive, just write the fuses, change to 3 to just read the fuses
  }
#endif

  if (doNext == 0)
  { // 0=display choices
    Serial.print("Selected mode: ");
    switch (mode)
    {
    case ATMEGA:
      Serial.println("ATMEGA");
      break;
    case TINY2313:
      Serial.println("ATtiny2313");
      break;
    case HVSP:
      Serial.println("ATtiny/HVSP");
      break;
    }
    Serial.println(F("Typical fuse values:"));
    Serial.println(F("          blank ATmega328P    UNO     Duemilanove328"));
    Serial.println(F("                    L 0x62   L 0xFF   L 0xFF"));
    Serial.println(F("                    H 0xD9   H 0xDE   H 0xDA"));
    Serial.println(F("                    X 0xFF   X 0xFD   X 0xFD"));
    Serial.print(F("\nProposed Low Fuse  "));
    Serial.print(F("hex: "));
    printHex(lfuse);
    Serial.print(F("  bin: "));
    printBits(lfuse);
    Serial.println();
    Serial.print(F("Proposed High Fuse "));
    Serial.print(F("hex: "));
    printHex(hfuse);
    Serial.print(F("  bin: "));
    printBits(hfuse);
    Serial.println();
    Serial.print(F("Proposed Ext Fuse  "));
    Serial.print(F("hex: "));
    printHex(efuse);
    Serial.print(F("  bin: "));
    printBits(efuse);
    Serial.println();
    Serial.println(F("\nCommands:"));
    Serial.println(F("1 select mode ATmegaXX8 28-pin"));
    Serial.println(F("2 select mode ATtiny2313"));
    Serial.println(F("3 select mode ATtiny 8-pin / HVSP"));
    Serial.println(F("L, H, or X followed by a number changes the proposed fuse"));
    Serial.println(F("B or U changes the proposed fuses to Blank or Uno"));
    Serial.println(F("R reads the current fuses from the target AVR"));
    Serial.println(F("W writes the proposed fuses to the target AVR"));
    Serial.println(F("Number format accepted is hex in the form FF or 0xFF"));
    Serial.println(F("    or decimal in the form NUM255"));
    Serial.println(F("    or binary in the form BIN11111111"));
    Serial.println(F("Upper or lower case is acceptable"));
    doNext = 1;
  }
  if (doNext == 1)
  { // 1=prompt for a command
    Serial.println(F("\nEnter a command:"));
    doNext = 2;
  }
  // 2=do nothing and expect further input, no routine for it, just skip through the loop

  if (doNext == 3)
  { // 3=read fuses from target AVR
    Serial.end();
    delay(50);
    // Initialize pins to enter programming mode

    // clear digital pins 0-7
    digitalWrite(DATA0, 0);
    digitalWrite(DATA1, 0);
    digitalWrite(DATA2, 0);
    digitalWrite(DATA3, 0);
    digitalWrite(DATA4, 0);
    digitalWrite(DATA5, 0);
    digitalWrite(DATA6, 0);
    digitalWrite(DATA7, 0);

    // set digital pins 0-7 as inputs for now
    pinMode(DATA0, INPUT);
    pinMode(DATA1, INPUT);
    pinMode(DATA2, INPUT);
    pinMode(DATA3, INPUT);
    pinMode(DATA4, INPUT);
    pinMode(DATA5, INPUT);
    pinMode(DATA6, INPUT);
    pinMode(DATA7, INPUT);

    digitalWrite(PAGEL, LOW);
    digitalWrite(XA1, LOW);
    digitalWrite(XA0, LOW);
    digitalWrite(BS1, LOW);
    digitalWrite(BS2, LOW);
    digitalWrite(WR, LOW); // ATtiny2313 needs this to be low to enter programming mode, ATmega doesn't care
    digitalWrite(OE, LOW);

    if (mode == HVSP)
    {
      digitalWrite(SDI, LOW); // set necessary pin values to enter programming mode
      digitalWrite(SII, LOW);
      pinMode(SDO, OUTPUT);   // SDO is same as RDY pin
      digitalWrite(SDO, LOW); // needs to be low to enter programming mode
    }

    // Enter programming mode
    digitalWrite(VCC, HIGH); // Apply VCC to start programming process
    delayMicroseconds(80);
    digitalWrite(RST, LOW); // Apply 12V to !RESET thru transistor

    if (mode == HVSP)
    {
      // reset SDO after short delay, longer leads to logic contention because target sets SDO high after entering programming mode
      delayMicroseconds(1); // datasheet says 10us, 1us is needed to avoid drive contention on SDO
      pinMode(SDO, INPUT);  // set to input to avoid logic contention
    }

    delayMicroseconds(10); // Give lots of time for part to enter programming mode
    digitalWrite(OE, HIGH);
    digitalWrite(WR, HIGH); // Now that we're in programming mode we can disable !WR
    delay(1);

    // Now in programming mode until RST is set HIGH again to remove 12V

    if (mode == HVSP)
    {
      HVSP_read(HVSP_READ_LFUSE_DATA, HVSP_READ_LFUSE_INSTR1);
      HVSP_read(0x00, HVSP_READ_LFUSE_INSTR2);
      read_lfuse = HVSP_read(0x00, HVSP_READ_LFUSE_INSTR3);

      HVSP_read(HVSP_READ_HFUSE_DATA, HVSP_READ_HFUSE_INSTR1);
      HVSP_read(0x00, HVSP_READ_HFUSE_INSTR2);
      read_hfuse = HVSP_read(0x00, HVSP_READ_HFUSE_INSTR3);

#if (BURN_EFUSE == 1)
      HVSP_read(HVSP_READ_EFUSE_DATA, HVSP_READ_EFUSE_INSTR1);
      HVSP_read(0x00, HVSP_READ_EFUSE_INSTR2);
      read_efuse = HVSP_read(0x00, HVSP_READ_EFUSE_INSTR3);
#endif
    }
    else
    {

      // Get current fuse settings stored on target device
      read_lfuse = fuse_read(LFUSE_SEL);
      read_hfuse = fuse_read(HFUSE_SEL);
#if (BURN_EFUSE == 1)
      read_efuse = fuse_read(EFUSE_SEL);
#endif
    }

    // done, disable outputs

    // clear digital pins 0-7
    digitalWrite(DATA0, 0);
    digitalWrite(DATA1, 0);
    digitalWrite(DATA2, 0);
    digitalWrite(DATA3, 0);
    digitalWrite(DATA4, 0);
    digitalWrite(DATA5, 0);
    digitalWrite(DATA6, 0);
    digitalWrite(DATA7, 0);

    // set digital pins 0-7 as inputs for now
    pinMode(DATA0, INPUT);
    pinMode(DATA1, INPUT);
    pinMode(DATA2, INPUT);
    pinMode(DATA3, INPUT);
    pinMode(DATA4, INPUT);
    pinMode(DATA5, INPUT);
    pinMode(DATA6, INPUT);
    pinMode(DATA7, INPUT);

    digitalWrite(RST, HIGH); // exit programming mode
    delay(1);
    digitalWrite(OE, LOW);
    digitalWrite(WR, LOW);
    digitalWrite(PAGEL, LOW);
    digitalWrite(XA1, LOW);
    digitalWrite(XA0, LOW);
    digitalWrite(BS1, LOW);
    digitalWrite(BS2, LOW);
    digitalWrite(VCC, LOW);

    // Open serial port again to print fuse values
    Serial.begin(BAUD);
    delay(50);
    Serial.println();
    Serial.print(F("Read Low Fuse  "));
    Serial.print(F("hex: "));
    printHex(read_lfuse);
    Serial.print(F("  bin: "));
    printBits(read_lfuse);
    Serial.println();
    Serial.print(F("Read High Fuse "));
    Serial.print(F("hex: "));
    printHex(read_hfuse);
    Serial.print(F("  bin: "));
    printBits(read_hfuse);
    Serial.println();

#if (BURN_EFUSE == 1)
    Serial.print(F("Read Ext Fuse  "));
    Serial.print(F("hex: "));
    printHex(read_efuse);
    Serial.print(F("  bin: "));
    printBits(read_efuse);
    Serial.println();
#endif
    Serial.println();

    doNext = 1;
  } // end of doNext3 read sequence

  if (doNext == 4)
  { // 4=write fuses to target AVR

    // This business with TXC0 is required because Arduino doesn't give us a means to tell if a serial
    // transmission is complete before we move on and do other things.  If we don't wait for TXC0 to be reset,
    // I found that sometimes the 1st fuse burn would fail.  It turns out that DATA1 (which doubles as Arduino serial
    // TX) was still toggling by the time the 1st XTAL strobe latches the fuse program command.  Bad news.
#ifndef __AVR_ATmega32U4__
    UCSR0A |= _BV(TXC0); // Reset serial transmit complete flag (need to do this manually because TX interrupts aren't used by Arduino)
#endif
    Serial.println(F("Burning fuses"));
#ifndef __AVR_ATmega32U4__
    while (!(UCSR0A & _BV(TXC0)))
      ; // Wait for serial transmission to complete before burning fuses!
#endif
    Serial.end(); // We're done with serial comms (for now) so disable UART
    delay(50);
    // Initialize pins to enter programming mode

    // clear digital pins 0-7
    digitalWrite(DATA0, 0);
    digitalWrite(DATA1, 0);
    digitalWrite(DATA2, 0);
    digitalWrite(DATA3, 0);
    digitalWrite(DATA4, 0);
    digitalWrite(DATA5, 0);
    digitalWrite(DATA6, 0);
    digitalWrite(DATA7, 0);

    // set digital pins 0-7 as inputs for now
    pinMode(DATA0, INPUT);
    pinMode(DATA1, INPUT);
    pinMode(DATA2, INPUT);
    pinMode(DATA3, INPUT);
    pinMode(DATA4, INPUT);
    pinMode(DATA5, INPUT);
    pinMode(DATA6, INPUT);
    pinMode(DATA7, INPUT);

    digitalWrite(PAGEL, LOW);
    digitalWrite(XA1, LOW);
    digitalWrite(XA0, LOW);
    digitalWrite(BS1, LOW);
    digitalWrite(BS2, LOW);
    digitalWrite(WR, LOW); // ATtiny2313 needs this to be low to enter programming mode, ATmega doesn't care
    digitalWrite(OE, LOW);

    if (mode == HVSP)
    {
      digitalWrite(SDI, LOW); // set necessary pin values to enter programming mode
      digitalWrite(SII, LOW);
      pinMode(SDO, OUTPUT);   // SDO is same as RDY pin
      digitalWrite(SDO, LOW); // needs to be low to enter programming mode
    }

    // Enter programming mode
    digitalWrite(VCC, HIGH); // Apply VCC to start programming process
    delayMicroseconds(80);
    digitalWrite(RST, LOW); // Apply 12V to !RESET thru level shifter

    if (mode == HVSP)
    {
      // reset SDO after short delay, longer leads to logic contention because target sets SDO high after entering programming mode
      delayMicroseconds(1); // datasheet says 10us, 1us is needed to avoid drive contention on SDO
      pinMode(SDO, INPUT);  // set to input to avoid logic contention
    }

    delayMicroseconds(10); // Give lots of time for part to enter programming mode
    digitalWrite(OE, HIGH);
    digitalWrite(WR, HIGH); // Now that we're in programming mode we can disable !WR
    delay(1);

    // Now in programming mode until RST is set HIGH again to remove 12V

    // Now burn desired fuses
    // How we do this depends on which mode we're in
    if (mode == HVSP)
    {
      HVSP_write(HVSP_WRITE_LFUSE_DATA, HVSP_WRITE_LFUSE_INSTR1);
      HVSP_write(lfuse, HVSP_WRITE_LFUSE_INSTR2);
      HVSP_write(0x00, HVSP_WRITE_LFUSE_INSTR3);
      HVSP_write(0x00, HVSP_WRITE_LFUSE_INSTR4);
      while (digitalRead(SDO) == LOW)
        ; // wait until burn is done

      HVSP_write(HVSP_WRITE_HFUSE_DATA, HVSP_WRITE_HFUSE_INSTR1);
      HVSP_write(hfuse, HVSP_WRITE_HFUSE_INSTR2);
      HVSP_write(0x00, HVSP_WRITE_HFUSE_INSTR3);
      HVSP_write(0x00, HVSP_WRITE_HFUSE_INSTR4);
      while (digitalRead(SDO) == LOW)
        ;

#if (BURN_EFUSE == 1)
      HVSP_write(HVSP_WRITE_EFUSE_DATA, HVSP_WRITE_EFUSE_INSTR1);
      HVSP_write(efuse, HVSP_WRITE_EFUSE_INSTR2);
      HVSP_write(0x00, HVSP_WRITE_EFUSE_INSTR3);
      HVSP_write(0x00, HVSP_WRITE_EFUSE_INSTR4);
      while (digitalRead(SDO) == LOW)
        ;
#endif
    }
    else
    {
      //delay(10);

      // First, program HFUSE
      fuse_burn(hfuse, HFUSE_SEL);

      // Now, program LFUSE
      fuse_burn(lfuse, LFUSE_SEL);

#if (BURN_EFUSE == 1)
      // Lastly, program EFUSE
      fuse_burn(efuse, EFUSE_SEL);
#endif
    }

    Serial.begin(BAUD); // open serial port
    delay(50);
    Serial.println(); // flush out any garbage data on the link left over from programming
    Serial.println(F("Burn complete"));
    Serial.println();

    // done, disable outputs

    // clear digital pins 0-7
    digitalWrite(DATA0, 0);
    digitalWrite(DATA1, 0);
    digitalWrite(DATA2, 0);
    digitalWrite(DATA3, 0);
    digitalWrite(DATA4, 0);
    digitalWrite(DATA5, 0);
    digitalWrite(DATA6, 0);
    digitalWrite(DATA7, 0);

    // set digital pins 0-7 as inputs for now
    pinMode(DATA0, INPUT);
    pinMode(DATA1, INPUT);
    pinMode(DATA2, INPUT);
    pinMode(DATA3, INPUT);
    pinMode(DATA4, INPUT);
    pinMode(DATA5, INPUT);
    pinMode(DATA6, INPUT);
    pinMode(DATA7, INPUT);

    digitalWrite(RST, HIGH); // exit programming mode
    delay(1);
    digitalWrite(OE, LOW);
    digitalWrite(WR, LOW);
    digitalWrite(PAGEL, LOW);
    digitalWrite(XA1, LOW);
    digitalWrite(XA0, LOW);
    digitalWrite(BS1, LOW);
    digitalWrite(BS2, LOW);
    digitalWrite(VCC, LOW);

    doNext = 3; // read the fuses next
  }             //end of doNext4 read sequence

  // handle a string which came in via Serial and decide what to do next based on the input
  if (stringComplete)
  {
    evalString();
    if (inputString == "1")
    {
      Serial.println(F("ATMEGA mode"));
      PAGEL = PAGELM;
      BS2 = BS2M;
      mode = ATMEGA;
      doNext = 1;
      base = 99;
    }
    else if (inputString == "2")
    {
      Serial.println(F("ATtiny2313 mode"));
      // reassign PAGEL and BS2 to their combined counterparts on the '2313
      PAGEL = PAGELT;
      BS2 = BS2T;
      mode = TINY2313;
      doNext = 1;
      base = 99;
    }
    else if (inputString == "3")
    {
      Serial.println(F("ATtiny/HVSP mode"));
      PAGEL = PAGELM;
      BS2 = BS2M;
      mode = HVSP;
      doNext = 1;
      base = 99;
    }
    else if (inputString == "B")
    {
      Serial.println(F("Set Proposed Blank 328 Fuses"));
      lfuse = LFUSE;
      hfuse = HFUSE;
      efuse = EFUSE;
      doNext = 0;
      base = 99;
    }
    else if (inputString == "U")
    {
      Serial.println(F("Set Proposed Uno Fuses"));
      lfuse = 0xFF;
      hfuse = 0xDE;
      efuse = 0xFD;
      doNext = 0;
      base = 99;
    }
    else if (inputString.startsWith("L", 0))
    {
      Serial.print(F("Proposed Low Fuse  "));
      inputString = inputString.substring(1);
      evalString();
      if (base > 0)
      {
        lfuse = val1;
        doNext = 1;
      }
    }
    else if (inputString.startsWith("H", 0))
    {
      Serial.print(F("Proposed High Fuse "));
      inputString = inputString.substring(1);
      evalString();
      if (base > 0)
      {
        hfuse = val1;
        doNext = 1;
      }
    }
    else if (inputString.startsWith("X", 0))
    {
      Serial.print(F("Proposed Ext Fuse  "));
      inputString = inputString.substring(1);
      evalString();
      if (base > 0)
      {
        efuse = val1;
        doNext = 1;
      }
    }
    if (base > 0 && base < 20)
    {
      Serial.print(F("hex: "));
      printHex(val1);
      Serial.print(F("  bin: "));
      printBits(val1);
      Serial.println();
    }
    else if (base == 99)
      ; // print nothing
    else if (inputString == "W")
    {
      Serial.println(F("Write"));
      doNext = 4;
    }
    else if (inputString == "R")
    {
      Serial.println(F("Read"));
      doNext = 3;
    }
    else
    {
      Serial.println("Invalid input");
      doNext = 0;
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
    base = 0;
  }

  // get new serial input and store in a String
  getSerialCharacters();

} //end of loop
