
void printHex(byte myByte)
{
    Serial.print(myByte / 16, HEX);
    Serial.print(myByte % 16, HEX);
}

void printBits(byte myByte)
{
    for (byte mask = 0x80; mask; mask >>= 1)
    {
        if (mask & myByte)
            Serial.print('1');
        else
            Serial.print('0');
    }
}

void getSerialCharacters()
{ // get serial input
    static unsigned long serTimeoutCounter;
    if (Serial.available())
    {
        char inChar = (char)Serial.read();
        if (inChar == '\n' && inputString != "")
        {
            stringComplete = true;
        }
        else
        {
            if (inChar > 32 && inputString.length() < 25)
            {
                inputString += inChar;
                serTimeoutCounter = millis();
            }
        }
    }
    if (millis() > SERIALTIMEOUT &&
        serTimeoutCounter < millis() - SERIALTIMEOUT &&
        inputString != "")
    {
        stringComplete = true;
    }
} // end of getSerialCharacters

void evalString()
{ // interpret the numbers and convert from hex, decimal, or binary as needed
    inputString.toUpperCase();
    //  Serial.print("String: ");
    //  Serial.println(inputString);

    if (inputString.startsWith("NUM", 0) &&
        (inputString.substring(3, 4) >= "0" && inputString.substring(3, 4) <= "9"))
    {
        //    Serial.println("decimal number");
        base = 10;
        inputString = inputString.substring(3);
    }
    else if (inputString.startsWith("BIN", 0) &&
             (inputString.substring(3, 4) >= "0" && inputString.substring(3, 4) <= "1"))
    {
        //    Serial.println("binary number");
        base = 2;
        inputString = inputString.substring(3);
    }
    else if (inputString.startsWith("0X", 0) &&
             ((inputString.substring(2, 3) >= "0" && inputString.substring(2, 3) <= "9") ||
              (inputString.substring(2, 3) >= "A" && inputString.substring(2, 3) <= "F")))
    {
        //    Serial.println("hex number");
        base = 16;
    }
    else if ((inputString.substring(0, 1) >= "0" && inputString.substring(0, 1) <= "9") ||
             (inputString.substring(0, 1) >= "A" && inputString.substring(0, 1) <= "F"))
    {
        //    Serial.println("hex number");
        base = 16;
    }

    int stringlength = inputString.length() + 1;
    char char_array[stringlength];
    inputString.toCharArray(char_array, stringlength);
    if (base != 0)
    {
        char *pEnd;
        val1 = strtol(char_array, &pEnd, base);
        if (val1 > 255)
        {
            base = 0;
        }
    }
} // end of evalString

void strobe_xtal(void)
{ // strobe xtal (usually to latch data on the bus)

    delay(1);
    digitalWrite(XTAL1, HIGH); // pulse XTAL to send command to target
    delay(1);
    digitalWrite(XTAL1, LOW);
}

void send_cmd(byte command) // Send command to target AVR
{
    // Set controls for command mode
    digitalWrite(XA1, HIGH);
    digitalWrite(XA0, LOW);
    digitalWrite(BS1, LOW);

    if (mode != TINY2313)
        digitalWrite(BS2, LOW); // Command load seems not to work if BS2 is high

    // set data pins to the value of command;
    digitalWrite(DATA0, bitRead(command, 0));
    digitalWrite(DATA1, bitRead(command, 1));
    digitalWrite(DATA2, bitRead(command, 2));
    digitalWrite(DATA3, bitRead(command, 3));
    digitalWrite(DATA4, bitRead(command, 4));
    digitalWrite(DATA5, bitRead(command, 5));
    digitalWrite(DATA6, bitRead(command, 6));
    digitalWrite(DATA7, bitRead(command, 7));

    // Set all DATA lines to outputs
    pinMode(DATA0, OUTPUT);
    pinMode(DATA1, OUTPUT);
    pinMode(DATA2, OUTPUT);
    pinMode(DATA3, OUTPUT);
    pinMode(DATA4, OUTPUT);
    pinMode(DATA5, OUTPUT);
    pinMode(DATA6, OUTPUT);
    pinMode(DATA7, OUTPUT);

    strobe_xtal(); // latch DATA

    digitalWrite(DATA0, 0);
    digitalWrite(DATA1, 0);
    digitalWrite(DATA2, 0);
    digitalWrite(DATA3, 0);
    digitalWrite(DATA4, 0);
    digitalWrite(DATA5, 0);
    digitalWrite(DATA6, 0);
    digitalWrite(DATA7, 0);

    // reset DATA to input to avoid bus contentions
    pinMode(DATA0, INPUT);
    pinMode(DATA1, INPUT);
    pinMode(DATA2, INPUT);
    pinMode(DATA3, INPUT);
    pinMode(DATA4, INPUT);
    pinMode(DATA5, INPUT);
    pinMode(DATA6, INPUT);
    pinMode(DATA7, INPUT);
}

void fuse_burn(byte fuse, int select) // write high or low fuse to AVR
{

    send_cmd(B01000000); // Send command to enable fuse programming mode

    // Enable data loading
    digitalWrite(XA1, LOW);
    digitalWrite(XA0, HIGH);
    // Specify low byte
    digitalWrite(BS1, LOW);
    if (mode != TINY2313)
        digitalWrite(BS2, LOW);
    delay(1);

    // Load fuse value into target

    // set data pins to value of fuse;
    digitalWrite(DATA0, bitRead(fuse, 0));
    digitalWrite(DATA1, bitRead(fuse, 1));
    digitalWrite(DATA2, bitRead(fuse, 2));
    digitalWrite(DATA3, bitRead(fuse, 3));
    digitalWrite(DATA4, bitRead(fuse, 4));
    digitalWrite(DATA5, bitRead(fuse, 5));
    digitalWrite(DATA6, bitRead(fuse, 6));
    digitalWrite(DATA7, bitRead(fuse, 7));

    // Set all DATA lines to outputs
    pinMode(DATA0, OUTPUT);
    pinMode(DATA1, OUTPUT);
    pinMode(DATA2, OUTPUT);
    pinMode(DATA3, OUTPUT);
    pinMode(DATA4, OUTPUT);
    pinMode(DATA5, OUTPUT);
    pinMode(DATA6, OUTPUT);
    pinMode(DATA7, OUTPUT);

    strobe_xtal(); // latch DATA

    digitalWrite(DATA0, 0);
    digitalWrite(DATA1, 0);
    digitalWrite(DATA2, 0);
    digitalWrite(DATA3, 0);
    digitalWrite(DATA4, 0);
    digitalWrite(DATA5, 0);
    digitalWrite(DATA6, 0);
    digitalWrite(DATA7, 0);

    // reset DATA to input to avoid bus contentions
    pinMode(DATA0, INPUT);
    pinMode(DATA1, INPUT);
    pinMode(DATA2, INPUT);
    pinMode(DATA3, INPUT);
    pinMode(DATA4, INPUT);
    pinMode(DATA5, INPUT);
    pinMode(DATA6, INPUT);
    pinMode(DATA7, INPUT);

    // Decide which fuse location to burn
    switch (select)
    {
    case HFUSE_SEL:
        digitalWrite(BS1, HIGH); // program HFUSE
        digitalWrite(BS2, LOW);
        break;
    case LFUSE_SEL:
        digitalWrite(BS1, LOW); // program LFUSE
        digitalWrite(BS2, LOW);
        break;
    case EFUSE_SEL:
        digitalWrite(BS1, LOW); // program EFUSE
        digitalWrite(BS2, HIGH);
        break;
    }
    delay(1);
    // Burn the fuse
    digitalWrite(WR, LOW);
    delay(1);
    digitalWrite(WR, HIGH);
    //delay(100);

    while (digitalRead(RDY) == LOW)
        ; // when RDY goes high, burn is done

    // Reset control lines to original state
    digitalWrite(BS1, LOW);
    digitalWrite(BS2, LOW);
}

byte fuse_read(int select)
{
    byte fuse;

    send_cmd(B00000100); // Send command to read fuse bits

    // Configure DATA as input so we can read back fuse values from target

    digitalWrite(DATA0, 0);
    digitalWrite(DATA1, 0);
    digitalWrite(DATA2, 0);
    digitalWrite(DATA3, 0);
    digitalWrite(DATA4, 0);
    digitalWrite(DATA5, 0);
    digitalWrite(DATA6, 0);
    digitalWrite(DATA7, 0);

    pinMode(DATA0, INPUT);
    pinMode(DATA1, INPUT);
    pinMode(DATA2, INPUT);
    pinMode(DATA3, INPUT);
    pinMode(DATA4, INPUT);
    pinMode(DATA5, INPUT);
    pinMode(DATA6, INPUT);
    pinMode(DATA7, INPUT);

    // Set control lines
    switch (select)
    {
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

    // read data pins and set value in variable fuse
    fuse = digitalRead(DATA7) << 7 |
           digitalRead(DATA6) << 6 |
           digitalRead(DATA5) << 5 |
           digitalRead(DATA4) << 4 |
           digitalRead(DATA3) << 3 |
           digitalRead(DATA2) << 2 |
           digitalRead(DATA1) << 1 |
           digitalRead(DATA0);

    digitalWrite(OE, HIGH); // Done reading, disable output enable line
    return fuse;
}

void sclk(void)
{ // send serial clock pulse, used by HVSP commands

    // These delays are much  longer than the minimum requirements,
    // but we don't really care about speed.
    delay(1);
    digitalWrite(SCI, HIGH);
    delay(1);
    digitalWrite(SCI, LOW);
}

byte HVSP_read(byte data, byte instr)
{ // Read a byte using the HVSP protocol

    byte response = 0x00; // a place to hold the response from target

    digitalWrite(SCI, LOW); // set clock low
    // 1st bit is always zero
    digitalWrite(SDI, LOW);
    digitalWrite(SII, LOW);
    sclk();

    // We capture a response on every readm even though only certain responses contain
    // valid data.  For fuses, the valid response is captured on the 3rd instruction write.
    // It is up to the program calling this function to figure out which response is valid.

    // The MSB of the response byte comes "early", that is,
    // before the 1st non-zero-padded byte of the 3rd instruction is sent to the target.
    // For more information, see the ATtiny25/45/85 datasheet, Table 20-16 (pg. 164).
    if (digitalRead(SDO) == HIGH) // target sent back a '1'?
        response |= 0x80;         // set MSB of response byte high

    // Send each bit of data and instruction byte serially, MSB first
    // I do this by shifting the byte left 1 bit at a time and checking the value of the new MSB
    for (int i = 0; i < 8; i++)
    {                                // i is bit number
        if ((data << i) & 0x80)      // shift data byte left and check if new MSB is 1 or 0
            digitalWrite(SDI, HIGH); // bit was 1, set pin high
        else
            digitalWrite(SDI, LOW); // bit was 0, set pin low

        if ((instr << i) & 0x80) // same process for instruction byte
            digitalWrite(SII, HIGH);
        else
            digitalWrite(SII, LOW);
        sclk();

        if (i < 7)
        { // remaining 7 bits of response are read here (one at a time)
            // note that i is one less than the bit position of response we are reading, since we read
            // the MSB above.  That's why I shift 0x40 right and not 0x80.
            if (digitalRead(SDO) == HIGH) // if we get a logic 1 from target,
                response |= (0x40 >> i);  // set corresponding bit of response to 1
        }
    }

    // Last 2 bits are always zero
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(SDI, LOW);
        digitalWrite(SII, LOW);
        sclk();
    }

    return response;
}

void HVSP_write(byte data, byte instr)
{ // Write to target using the HVSP protocol

    digitalWrite(SCI, LOW); // set clock low

    // 1st bit is always zero
    digitalWrite(SDI, LOW);
    digitalWrite(SII, LOW);
    sclk(); // latch bit

    // Send each bit of data and instruction byte serially, MSB first
    // I do this by shifting the byte left 1 bit at a time and checking the value of the new MSB
    for (int i = 0; i < 8; i++)
    {                                // i is bit number
        if ((data << i) & 0x80)      // shift data byte left and check if new MSB is 1 or 0
            digitalWrite(SDI, HIGH); // bit was 1, set pin high
        else
            digitalWrite(SDI, LOW); // bit was 0, set pin low

        if ((instr << i) & 0x80) // same process for instruction byte
            digitalWrite(SII, HIGH);
        else
            digitalWrite(SII, LOW);

        sclk(); // strobe SCI (serial clock) to latch data
    }

    // Last 2 bits are always zero
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(SDI, LOW);
        digitalWrite(SII, LOW);
        sclk();
    }
}

