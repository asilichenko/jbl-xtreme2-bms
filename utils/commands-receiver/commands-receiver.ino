/**
  @file commands-receiver.ino

  This program receives commands from the speaker and prints them into Serial port.

  Arduino Nano:
  - A4 = SDA -> white
  - A5 = SCL -> yellow


  MIT License

  Copyright (c) 2024 Oleksii Sylichenko

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <Wire.h>

/**
  @brief Standard BQ28Z610 Device I2C Address
  @see 12.1 Standard Data Commands https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
*/
const byte DEVICE_ADDR = 0x55;

void setup() {
  Wire.begin(DEVICE_ADDR);  // join i2c bus with address
  Wire.onReceive(receiveEvent);  // register on receive event

  Serial.begin(9600);  // start serial for output
  while (!Serial);
  Serial.println("============ START: [JBL XTREAME 2 Commands Receiver] ============");
  Serial.println("License: MIT License");
  Serial.println("Copyright (c) 2024 Oleksii Sylichenko");
  Serial.println("==================================================================");
}

/**
  Print byte value in the format: "0x0A".
*/
void printByteHex(byte val) {
  Serial.print("0x");
  if (val <= 0xF) Serial.print('0');
  Serial.print(val, HEX);
}

/**
  Function that executes whenever data is received from master
  this function is registered as an event, see setup().

  Received commands:
  - 0x06 - 12.1.4 0x06/07 Temperature()
  - 0x08 - 12.1.5 0x08/09 Voltage()
  - 0x2C - 12.1.23 0x2C/2D RelativeStateOfCharge()

  @see https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
*/
void receiveEvent(int howMany) {
  while (Wire.available()) {
    const byte bt = Wire.read();
    Serial.print("received: ");
    printByteHex(bt);
  }
  Serial.println();
}

void loop() {
}
