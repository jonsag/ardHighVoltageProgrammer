/*
  Title:        HVRescue_Shield
  Description:  Arduino sketch for use with the HV Rescue Shield
  Author:       Jeff Keyzer
  Company:      MightyOhm Engineering
  Website:      http://mightyohm.com
  Contact:      http://mightyohm.com/blog/contact/  
  
  This sketch assumes that the Arduino is equipped with an AVR HV Rescue Shield.
  Schematics and other details are available at http://mightyohm.com/hvrescue2

  The sketch uses High Voltage Programming Mode to set fuses on many Atmel AVR family 8-bit microcontrollers.
  Version 2.0 adds support for High Voltage Serial Programming (HVSP) mode and 8-pin ATtiny devices, but remains
  backwards compatible with the 1.x series hardware.
  
  The HVPP routines are based on those described in the ATmega48/88/168 datasheet rev.
  2545M-AVR-09/07, pg. 290-297 and the ATtiny2313 datasheet rev. 2543I-AVR-04/06 pg. 165-176.  
  
  The HVSP routines are based on the ATtiny25/45/85 and 13A datasheets (ATtiny25/45/85 2586M–AVR–07/10 pg. 159-165,
  ATtiny13A 8126E-AVR-7/10 pg. 109-116).
  
  These routines are compatible with many other members of the AVR family that are not listed here.
  For a complete list of tested microcontrollers, see http://mightyohm.com/wiki/products:hvrescue:compatibility
  
  Changelog:
  3/15/11 2.12
    - New digital pin 0-7 (command&data) read/write routines for the Arduino Mega, since these lines are implemented
      differently on the Mega than on the original Arduino.
      
  3/8/11 2.11
    - Analog inputs (used here as digital outputs) are now called by their new Arduino A0-A5 names.
      
  2/2/11 2.1
    - adjusted RESET and VCC edge timing to work with new board design and avoid signal contention on SDO
    - fixed bug that prevented program from compiling in non-interactive mode
    - modified non-interactive mode so that read/verify serial comms still occur, but fuse values aren't prompted
  
  12/17/10 2.01
    - added missing braces to if(mode == HVSP) that sets SDO pinmode to INPUT
    - removed misleading comment about removing AVR when entering fuse values
    - default mode changed back to ATMEGA
  
  12/13/10 v2.0
    - Added support for 8-pin parts that use HV Serial Programming (HVSP)
    - New mode selection at startup determines which type of part is to be programmed 
    - Got rid of endSerial function, since Arduino now includes Serial.end (finally!)
    - Added a wait for serial transmit to complete before burning fuses.  Without this HFUSE burn would fail occasionally.
    - Numerous other minor tweaks, removal of unnecessary delays, better commenting
    
  9/24/10 v1.2a
    - ATtiny2313 mode was being set by default.  Changed default mode back to ATmega (see #define ATtiny).
    
  8/16/10 v1.2
    - Existing fuse settings are now shown before asking the user for new values
    - Added OE strobe after entering programming mode to get ATtiny2313 to read first fuse correctly.
    - Cleaned up code a bit
    - Some minor tweaks to data direction register settings during setup, etc.
    
  11/02/09 v1.1
    - Removed endSerial call after reading back fuse bytes, was spewing garbage into
      serial monitor
    - Still occasionally get garbage when opening serial monitor, not sure what is causing this.
    
  03/01/09 v1.0
    - ATtiny2313 support, enable with ATtiny option
    - 12V Step up converter enable is non-inverting, unlike previous level shifter circuit
    - added interactive mode, asks for fuse values to burn, option to turn off
    - added EFUSE support and option to enable
    - button now has very simple debounce routine
    
  09/24/08
    - original release of sketch "HVFuse" to support first implementation on perfboard
    - Details: http://mightyohm.com/blog/2008/09/arduino-based-avr-high-voltage-programmer/
  
  Copyright 2008, 2009, 2010 Jeff Keyzer
 
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "modules.h"

void setup() { // run once, when the sketch starts

  byte response = NULL;  // user response from mode query
  
  // Set up control lines for HV parallel programming
  
  #if (MEGA == 0)  // Set up data lines on original Arduino
    PORTD = 0x00;  // clear digital pins 0-7
    DDRD = 0x00;  // set digital pins 0-7 as inputs for now
  #else  // Mega uses different ports for same pins, much more complicated setup.  yuck!
     mega_data_input();
  #endif
  
  pinMode(VCC, OUTPUT);
  pinMode(RDY, INPUT);
  pinMode(OE, OUTPUT);
  pinMode(WR, OUTPUT);
  pinMode(BS1, OUTPUT);
  pinMode(XA0, OUTPUT);
  pinMode(XA1, OUTPUT);
  pinMode(PAGEL, OUTPUT);
  pinMode(RST, OUTPUT);  // enable signal for DC-DC converter that generates +12V !RESET
  pinMode(BS2, OUTPUT);
  pinMode(XTAL1, OUTPUT);
  
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);  // turn on internal pullup resistor

  // Initialize output pins as needed
  digitalWrite(RST, LOW);  // Turn off 12V step-up converter (non-inverting, unlike original circuit)
  digitalWrite(VCC, LOW); 

  Serial.begin(BAUD);  // Open serial port, this works on the Mega also because we are using serial port 0
  
    // Ask user which chip family we are programming
    #if ((ASKMODE == 1) && (INTERACTIVE == 1))
    Serial.println("Select mode:");
    Serial.println("1: ATmega (28-pin)");
    Serial.println("2: ATtiny2313");
    Serial.println("3: ATtiny (8-pin) / HVSP");
    
    while(response == NULL) {
      
      while (Serial.available() == 0);   // wait for character
      response = Serial.read();  // get response from user
    
      switch(response) {  // decide what to do
      case '1':
        mode = ATMEGA;
        break;
      case '2':
        mode = TINY2313;
        break;
      case '3':
        mode = HVSP;
        break;
      default:
        Serial.println("Invalid response.  Try again.");
        response = NULL;  // reset response so we go thru the while loop again
        break;
      } 
    }
    #endif
    
    // Report which mode was selected
    Serial.print("Selected mode: ");
    switch(mode) {
    case ATMEGA:
      Serial.println("ATMEGA");
      break;
    case TINY2313:
      Serial.println("ATtiny2313");
      // reassign PAGEL and BS2 to their combined counterparts on the '2313
      PAGEL = BS1;  
      BS2 = XA1;
      break;
    case HVSP:
      Serial.println("ATtiny/HVSP");
      break;
    }
}

void loop() {  // run over and over again
  
  byte hfuse, lfuse, efuse;  // desired fuse values from user
  byte read_hfuse, read_lfuse, read_efuse;  // fuses read from target for verify
  
  Serial.println("Insert target AVR and press button.");
  Serial.end();
  
  // Set lower 2 bits of DATA low.  This helps avoid serial garbage showing up when you insert a part.
  #if (MEGA == 0)
    DDRD = 0x03;
    PORTD = 0x00;
  #else  // Lower 2 bits are part of PORTE on the Mega
    PORTE &= ~(_BV(PE0) | _BV(PE1));
    DDRE |= (_BV(PE0) | _BV(PE1));
  #endif

  // wait for button press, debounce
  while(1) {
    while (digitalRead(BUTTON) == HIGH);  // wait here until button is pressed
    delay(100);                            // simple debounce routine
    if (digitalRead(BUTTON) == LOW)       // if the button is still pressed, continue
      break;  // valid press was detected, continue on with rest of program
  }
  // Initialize pins to enter programming mode
  #if (MEGA == 0)  // Set up data lines on original Arduino
    PORTD = 0x00;  // clear digital pins 0-7
    DDRD = 0x00;  // set digital pins 0-7 as inputs for now
  #else
    mega_data_input();
  #endif
  digitalWrite(PAGEL, LOW);
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
  digitalWrite(WR, LOW);  // ATtiny2313 needs this to be low to enter programming mode, ATmega doesn't care
  digitalWrite(OE, LOW);
 
  if(mode == HVSP) {
    digitalWrite(SDI, LOW);  // set necessary pin values to enter programming mode
    digitalWrite(SII, LOW);
    pinMode(SDO, OUTPUT);    // SDO is same as RDY pin
    digitalWrite(SDO, LOW);  // needs to be low to enter programming mode
  }
  
  // Enter programming mode
  digitalWrite(VCC, HIGH);  // Apply VCC to start programming process
  delayMicroseconds(80);
  digitalWrite(RST, HIGH);   // Apply 12V to !RESET thru level shifter
  
  if(mode == HVSP) {
    // reset SDO after short delay, longer leads to logic contention because target sets SDO high after entering programming mode
    delayMicroseconds(1);   // datasheet says 10us, 1us is needed to avoid drive contention on SDO 
    pinMode(SDO, INPUT);    // set to input to avoid logic contention
  }
  
  delayMicroseconds(10);  // Give lots of time for part to enter programming mode
  digitalWrite(OE, HIGH);
  digitalWrite(WR, HIGH);   // Now that we're in programming mode we can disable !WR
  delay(1);
  
  /****
   **** Now we're in programming mode until RST is set LOW again
   ****/
  
  if (mode == HVSP) {
    HVSP_read(HVSP_READ_LFUSE_DATA, HVSP_READ_LFUSE_INSTR1);
    HVSP_read(0x00, HVSP_READ_LFUSE_INSTR2);
    read_lfuse=HVSP_read(0x00, HVSP_READ_LFUSE_INSTR3);
  
    HVSP_read(HVSP_READ_HFUSE_DATA, HVSP_READ_HFUSE_INSTR1);
    HVSP_read(0x00, HVSP_READ_HFUSE_INSTR2);
    read_hfuse=HVSP_read(0x00, HVSP_READ_HFUSE_INSTR3);
    
    #if (BURN_EFUSE == 1)
      HVSP_read(HVSP_READ_EFUSE_DATA, HVSP_READ_EFUSE_INSTR1);
      HVSP_read(0x00, HVSP_READ_EFUSE_INSTR2);
      read_efuse=HVSP_read(0x00, HVSP_READ_EFUSE_INSTR3);
    #endif
    
  } else {
    
    // Get current fuse settings stored on target device 
    read_lfuse = fuse_read(LFUSE_SEL);
    read_hfuse = fuse_read(HFUSE_SEL);
    #if (BURN_EFUSE == 1)
      read_efuse = fuse_read(EFUSE_SEL);
    #endif
  }

  // Open serial port again to print fuse values
  Serial.begin(BAUD);
  Serial.print("\n");
  Serial.println("Existing fuse values:");
  Serial.print("LFUSE: ");
  Serial.println(read_lfuse, HEX);
  Serial.print("HFUSE: ");
  Serial.println(read_hfuse, HEX);
  #if (BURN_EFUSE == 1)
    Serial.print("EFUSE: ");
    Serial.println(read_efuse, HEX);
  #endif
  Serial.print("\n");  
 
  #if (INTERACTIVE == 1)
    // Ask the user what fuses should be burned to the target
    // For a guide to AVR fuse values, go to http://www.engbedded.com/cgi-bin/fc.cgi 
    Serial.print("Enter desired LFUSE hex value (ie. 0x62): ");
    lfuse = fuse_ask();
    Serial.print("Enter desired HFUSE hex value (ie. 0xDF): ");
    hfuse = fuse_ask(); 
    
    #if (BURN_EFUSE == 1)
      Serial.print("Enter desired EFUSE hex value (ie. 0xF9): ");
      efuse = fuse_ask();
    #endif 
    
  #else  // not using interactive mode, just set fuses to values defined in header
    hfuse = HFUSE;
    lfuse = LFUSE;
    efuse = EFUSE;
  #endif
  
  // This business with TXC0 is required because Arduino doesn't give us a means to tell if a serial 
  // transmission is complete before we move on and do other things.  If we don't wait for TXC0 to be reset,
  // I found that sometimes the 1st fuse burn would fail.  It turns out that DATA1 (which doubles as Arduino serial
  // TX) was still toggling by the time the 1st XTAL strobe latches the fuse program command.  Bad news.
    
  UCSR0A |= _BV(TXC0);  // Reset serial transmit complete flag (need to do this manually because TX interrupts aren't used by Arduino)
  Serial.println("Burning fuses...");
  while(!(UCSR0A & _BV(TXC0)));  // Wait for serial transmission to complete before burning fuses!
    
  Serial.end();    // We're done with serial comms (for now) so disable UART

  // Now burn desired fuses
  // How we do this depends on which mode we're in
  if (mode == HVSP) {
    HVSP_write(HVSP_WRITE_LFUSE_DATA, HVSP_WRITE_LFUSE_INSTR1);
    HVSP_write(lfuse, HVSP_WRITE_LFUSE_INSTR2);
    HVSP_write(0x00, HVSP_WRITE_LFUSE_INSTR3);
    HVSP_write(0x00, HVSP_WRITE_LFUSE_INSTR4);
    while(digitalRead(SDO) == LOW);  // wait until burn is done
    
    HVSP_write(HVSP_WRITE_HFUSE_DATA, HVSP_WRITE_HFUSE_INSTR1);
    HVSP_write(hfuse, HVSP_WRITE_HFUSE_INSTR2);
    HVSP_write(0x00, HVSP_WRITE_HFUSE_INSTR3);
    HVSP_write(0x00, HVSP_WRITE_HFUSE_INSTR4);
    while(digitalRead(SDO) == LOW);
    
    #if (BURN_EFUSE == 1)
      HVSP_write(HVSP_WRITE_EFUSE_DATA, HVSP_WRITE_EFUSE_INSTR1);
      HVSP_write(efuse, HVSP_WRITE_EFUSE_INSTR2);
      HVSP_write(0x00, HVSP_WRITE_EFUSE_INSTR3);
      HVSP_write(0x00, HVSP_WRITE_EFUSE_INSTR4);
      while(digitalRead(SDO) == LOW);
    #endif
    
    // Read back fuse contents to verify burn worked
    HVSP_read(HVSP_READ_LFUSE_DATA, HVSP_READ_LFUSE_INSTR1);
    HVSP_read(0x00, HVSP_READ_LFUSE_INSTR2);
    read_lfuse=HVSP_read(0x00, HVSP_READ_LFUSE_INSTR3);
    HVSP_read(HVSP_READ_HFUSE_DATA, HVSP_READ_HFUSE_INSTR1);
    HVSP_read(0x00, HVSP_READ_HFUSE_INSTR2);
    read_hfuse=HVSP_read(0x00, HVSP_READ_HFUSE_INSTR3);
  
    #if (BURN_EFUSE == 1)
      HVSP_read(HVSP_READ_EFUSE_DATA, HVSP_READ_EFUSE_INSTR1);
      HVSP_read(0x00, HVSP_READ_EFUSE_INSTR2);
      read_efuse=HVSP_read(0x00, HVSP_READ_EFUSE_INSTR3);
    #endif
    
    } else {
    //delay(10);
    
    // First, program HFUSE
    fuse_burn(hfuse, HFUSE_SEL);
  
    // Now, program LFUSE
    fuse_burn(lfuse, LFUSE_SEL);
  
    #if (BURN_EFUSE == 1)
      // Lastly, program EFUSE
      fuse_burn(efuse, EFUSE_SEL);
    #endif
  
    // Read back fuse contents to verify burn worked
    read_lfuse = fuse_read(LFUSE_SEL);
    read_hfuse = fuse_read(HFUSE_SEL);
  
    #if (BURN_EFUSE == 1)
      read_efuse = fuse_read(EFUSE_SEL);
    #endif

    // Done verifying
    digitalWrite(OE, HIGH);
  }
  
  Serial.begin(BAUD);  // open serial port
  Serial.print("\n");  // flush out any garbage data on the link left over from programming
  Serial.print("Read LFUSE: ");
  Serial.println(read_lfuse, HEX);
  Serial.print("Read HFUSE: ");
  Serial.println(read_hfuse, HEX);
  #if (BURN_EFUSE == 1)
    Serial.print("Read EFUSE: ");
    Serial.println(read_efuse, HEX);
  #endif  
  Serial.println("Burn complete."); 
  Serial.print("\n");
  Serial.println("It is now safe to remove the target AVR.");
  Serial.print("\n");
  
  // All done, disable outputs
  #if (MEGA == 0)  // Set up data lines on original Arduino
    PORTD = 0x00;  // clear digital pins 0-7
    DDRD = 0x00;  // set digital pins 0-7 as inputs for now
  #else
    mega_data_input();
  #endif
  digitalWrite(RST, LOW);  // exit programming mode
  delay(1);
  digitalWrite(OE, LOW);
  digitalWrite(WR, LOW);
  digitalWrite(PAGEL, LOW);
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
  digitalWrite(VCC, LOW);
}