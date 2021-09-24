
void strobe_xtal(void) {  // strobe xtal (usually to latch data on the bus)
  
  delay(1);
  digitalWrite(XTAL1, HIGH);  // pulse XTAL to send command to target
  delay(1);
  digitalWrite(XTAL1, LOW);
}

int hex2dec(byte c) { // converts one HEX character into a number
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }
}

void sclk(void) {  // send serial clock pulse, used by HVSP commands

  // These delays are much  longer than the minimum requirements,
  // but we don't really care about speed.
  delay(1);  
  digitalWrite(SCI, HIGH);
  delay(1);
  digitalWrite(SCI, LOW);
}

void send_cmd(byte command)  // Send command to target AVR
{
  // Set controls for command mode
  digitalWrite(XA1, HIGH);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);  
  
  if (mode != TINY2313)
    digitalWrite(BS2, LOW);  // Command load seems not to work if BS2 is high 
  

  #if (MEGA == 0)
    PORTD = command;
    DDRD = 0xFF;  // Set all DATA lines to outputs
  #else
    mega_data_write(command);
  #endif
  
  strobe_xtal();  // latch DATA
  
  #if (MEGA == 0)
    PORTD = 0x00;
    DDRD = 0x00;  // reset DATA to input to avoid bus contentions
  #else
    mega_data_input();
  #endif
}

void fuse_burn(byte fuse, int select)  // write high or low fuse to AVR
{
  
  send_cmd(B01000000);  // Send command to enable fuse programming mode
  
  // Enable data loading
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, HIGH);
  // Specify low byte
  digitalWrite(BS1, LOW); 
  if (mode != TINY2313)
    digitalWrite(BS2, LOW);  
  delay(1);
  
  // Load fuse value into target
  #if (MEGA == 0)
    PORTD = fuse;
    
    DDRD = 0xFF;  // Set all DATA lines to outputs
  #else
    mega_data_write(fuse);
  #endif
  
  strobe_xtal();  // latch DATA

  #if (MEGA == 0)
    PORTD = 0x00;
    DDRD = 0x00;  // reset DATA to input to avoid bus contentions
  #else
    mega_data_input();
  #endif
   
  // Decide which fuse location to burn
  switch (select) { 
  case HFUSE_SEL:
    digitalWrite(BS1, HIGH); // program HFUSE
    digitalWrite(BS2, LOW);
    break;
  case LFUSE_SEL:
    digitalWrite(BS1, LOW);  // program LFUSE
    digitalWrite(BS2, LOW);
    break;
  case EFUSE_SEL:
    digitalWrite(BS1, LOW);  // program EFUSE
    digitalWrite(BS2, HIGH);
    break;
  }
  delay(1);
   // Burn the fuse
  digitalWrite(WR, LOW); 
  delay(1);
  digitalWrite(WR, HIGH);
  //delay(100);
  
  while(digitalRead(RDY) == LOW);  // when RDY goes high, burn is done
  
  // Reset control lines to original state
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
}

byte fuse_read(int select) {
  byte fuse;
  
  send_cmd(B00000100);  // Send command to read fuse bits
  
  // Configure DATA as input so we can read back fuse values from target
  #if (MEGA == 0)
    PORTD = 0x00;
    DDRD = 0x00;  // reset DATA to input to avoid bus contentions
  #else
    mega_data_input();
  #endif

  // Set control lines
  switch (select) {
    case LFUSE_SEL:  
      // Read LFUSE
      digitalWrite(BS2, LOW);
      digitalWrite(BS1, LOW);
      break;
    case HFUSE_SEL:
      // Read HFUSE
      digitalWrite(BS2, HIGH);
      digitalWrite(BS1, HIGH);
      break;
    case EFUSE_SEL:
      // Read EFUSE
      digitalWrite(BS2, HIGH);
      digitalWrite(BS1, LOW);
      break;
  }
  
  //  Read fuse
  digitalWrite(OE, LOW);
  delay(1);
  
  #if (MEGA == 0)
    fuse = PIND;
  #else
    fuse = mega_data_read();
  #endif
  
  digitalWrite(OE, HIGH);  // Done reading, disable output enable line
  return fuse;
}

byte fuse_ask(void) {  // get desired fuse value from the user (via the serial port)
  byte incomingByte = 0;
  byte fuse;
  char serbuffer[2];
  
  while (incomingByte != 'x') {  // crude way to wait for a hex string to come in
    while (Serial.available() == 0);   // wait for a character to come in
    incomingByte = Serial.read();
  }
  
  // Hopefully the next two characters form a hex byte.  If not, we're hosed.
  while (Serial.available() == 0);   // wait for character
  serbuffer[0] = Serial.read();      // get high byte of fuse value
  while (Serial.available() == 0);   // wait for character
  serbuffer[1] = Serial.read();      // get low byte
  
  fuse = hex2dec(serbuffer[1]) + hex2dec(serbuffer[0]) * 16;
  
  Serial.println(fuse, HEX);  // echo fuse value back to the user
  
  return fuse;
  
}

byte HVSP_read(byte data, byte instr) { // Read a byte using the HVSP protocol

  byte response = 0x00; // a place to hold the response from target

  digitalWrite(SCI, LOW);  // set clock low
  // 1st bit is always zero
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  sclk();
  
  // We capture a response on every read even though only certain responses contain
  // valid data.  For fuses, the valid response is captured on the 3rd instruction write.
  // It is up to the program calling this function to figure out which response is valid.
  
  // The MSB of the response byte comes "early", that is, 
  // before the 1st non-zero-padded byte of the 3rd instruction is sent to the target.
  // For more information, see the ATtiny25/45/85 datasheet, Table 20-16 (pg. 164).
  if (digitalRead(SDO) == HIGH) // target sent back a '1'?
    response |= 0x80;  // set MSB of response byte high
  
  // Send each bit of data and instruction byte serially, MSB first
  // I do this by shifting the byte left 1 bit at a time and checking the value of the new MSB
  for (int i=0; i<8; i++) {  // i is bit number
    if ((data << i) & 0x80)  // shift data byte left and check if new MSB is 1 or 0
      digitalWrite(SDI, HIGH);  // bit was 1, set pin high
    else
      digitalWrite(SDI, LOW);   // bit was 0, set pin low
      
    if ((instr << i) & 0x80)   // same process for instruction byte
      digitalWrite(SII, HIGH);
    else
      digitalWrite(SII, LOW);
   sclk();
       
    if (i < 7) {  // remaining 7 bits of response are read here (one at a time)
      // note that i is one less than the bit position of response we are reading, since we read
      // the MSB above.  That's why I shift 0x40 right and not 0x80. 
      if(digitalRead(SDO) == HIGH)  // if we get a logic 1 from target,
        response |= (0x40 >> i);    // set corresponding bit of response to 1
    }
  }
  
  // Last 2 bits are always zero
  for (int i=0; i<2; i++) {
    digitalWrite(SDI, LOW);
    digitalWrite(SII, LOW);
    sclk();
  }
  
  return response;
}

void HVSP_write(byte data, byte instr) { // Write to target using the HVSP protocol

  digitalWrite(SCI, LOW);  // set clock low
  
  // 1st bit is always zero
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  sclk();  // latch bit
  
  // Send each bit of data and instruction byte serially, MSB first
  // I do this by shifting the byte left 1 bit at a time and checking the value of the new MSB
  for (int i=0; i<8; i++) {  // i is bit number
    if ((data << i) & 0x80)  // shift data byte left and check if new MSB is 1 or 0
      digitalWrite(SDI, HIGH);  // bit was 1, set pin high
    else
      digitalWrite(SDI, LOW);   // bit was 0, set pin low
      
    if ((instr << i) & 0x80)  // same process for instruction byte
      digitalWrite(SII, HIGH);
    else
      digitalWrite(SII, LOW);
      
   sclk();  // strobe SCI (serial clock) to latch data
  }
  
  // Last 2 bits are always zero
  for (int i=0; i<2; i++) {
    digitalWrite(SDI, LOW);
    digitalWrite(SII, LOW);   
    sclk();
  }
}

#if (MEGA == 1)  // functions specifically for the Arduino Mega

void mega_data_write(byte data) { // Write a byte to digital lines 0-7
  // This is really ugly, thanks to the way that digital lines 0-7 are implemented on the Mega.
  PORTE &= ~(_BV(PE0) | _BV(PE1) | _BV(PE4) | _BV(PE5) | _BV(PE3));  // clear bits associated with digital pins 0-1, 2-3, 5
  PORTE |= (data & 0x03);  // set lower 2 bits corresponding to digital pins 0-1
  PORTE |= (data & 0x0C) << 2;  // set PORTE bits 4-5, corresponding to digital pins 2-3
  PORTE |= (data & 0x20) >> 2;  // set PORTE bit 5, corresponding to digital pin 5
  DDRE |= (_BV(PE0) | _BV(PE1) | _BV(PE4) | _BV(PE5) | _BV(PE3));  // set bits we are actually using to outputs

  PORTG &= ~(_BV(PG5));  // clear bits associated with digital pins 4-5
  PORTG |= (data & 0x10) << 1;  // set PORTG bit 5, corresponding to digital pin 4
  DDRG |= (_BV(PG5));  // set to output

  PORTH &= ~(_BV(PH3) | _BV(PH4));  // clear bites associated with digital pins 6-7
  PORTH |= (data & 0xC0) >> 3;  // set PORTH bits 3-4, corresponding with digital pins 6-7
  DDRH |= (_BV(PH3) | _BV(PH4));  // set bits to outputs
}

byte mega_data_read(void) { // Read a byte from digital lines 0-7
  byte data = 0x00;  // initialize to zero
  data |= (PINE & 0x03);  // set lower 2 bits
  data |= (PINE & 0x30) >> 2;  // set bits 3-4 from PINE bits 4-5
  data |= (PINE & 0x08) << 2;  // set bit 5 from PINE bit 3
  data |= (PING & 0x20) >> 1;  // set bit 4 from PING bit 5
  data |= (PINH & 0x18) << 3;  // set bits 6-7 from PINH bits 3-4
    
  return data;
}

void mega_data_input(void) { // Set digital lines 0-7 to inputs and turn off pullups
  PORTE &= ~(_BV(PE0) | _BV(PE1) | _BV(PE4) | _BV(PE5) | _BV(PE3));  // Mega digital pins 0-3, 5
  DDRE &= ~(_BV(PE0) | _BV(PE1) | _BV(PE4) | _BV(PE5) | _BV(PE3));  // Set to input
  PORTG &= ~(_BV(PG5));  // Mega digital pin 4
  DDRG &= ~(_BV(PG5));  // Set to input
  PORTH &= ~(_BV(PH3) | _BV(PH4));  // Mega digital pins 6-7
  DDRH &= ~(_BV(PH3) | _BV(PH4));  // Set to input
}
#endif
