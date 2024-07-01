/**
  @file speaker_emulator.ino

  This program periodically sends commads to the battery pack gas gauging device BQ28Z610:
  - 12.1.4 0x06/07 Temperature()
  - 12.1.5 0x08/09 Voltage()
  - 12.1.23 0x2C/2D RelativeStateOfCharge()

  @see Texas Instruments Technical Reference Manual https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
  @see BQ28Z610 product: https://www.ti.com/product/BQ28Z610

  Commands Voltage() and RelativeStateOfCharge() are sent in pair once per 5 seconds.

  Command Temperature() is sent ones per 10s.

  There is could be any time shift between them.

  Connection

  Arduino Nano:
  - A4 = SDA -> white
  - A5 = SCL -> yellow

   AtTiny85:
   - 5 = SDA -> white
   - 7 = SCL -> yellow

  <pre>
      -|1* 8|- VCC
      -|2  7|- C (SCL, yellow)
      -|3  6|-
  GND -|4  5|- D (SDA, white)
  </pre>


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
  @see 12.1 Standard Data Commands
*/
#define DEVICE_ADDR 0x55

/**
  @brief 12.1 Standard Data Commands
  - 12.1.4 0x06/07 Temperature()
  - 12.1.5 0x08/09 Voltage()
  - 12.1.23 0x2C/2D RelativeStateOfCharge()
  @see https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
*/
class Commands {
  public:
    static const byte TEMP = 0x06;  // 12.1.4 0x06/07 Temperature()
    static const byte VOLT = 0x08;  // 12.1.5 0x08/09 Voltage()
    static const byte RSOC = 0x2C;  // 12.1.23 0x2C/2D RelativeStateOfCharge()
};

/**
  Delay between sending pair of commands Voltage() and RelativeStateOfCharge().

  Standard value is 5 seconds.
*/
const int VOLT_RSOC_DELAY = 5000;

/**
  Delay between sending command Temperature().

  Standard value is 10 seconds.
*/
const int TEMP_DELAY = 10000;

/**
  Offset between sending pair of cammands Voltage() and RelativeStateOfCharge() and sending command Temperature().

  This value may change.
*/
const int TIME_OFFSET_VOLT_RSOC_TEMP = 1000;

void printByte(byte val) {
  if (val < 0xF) Serial.print('0');
  Serial.print(val, HEX);
}

void printData(byte data[]) {
  Serial.print("[ ");
  printByte(data[0]);
  Serial.print(' ');
  printByte(data[1]);
  Serial.print(" ]");
}

/**
  @brief Compose word value from two bytes in Little-endian.

  [0x34, 0x12] -> 0x1234
*/
word composeWord(byte data[]) {
  const byte LSB = data[0];
  const byte MSB = data[1];
  return (MSB << 8) | LSB;
}

void setup() {
  Wire.begin(); // join i2c bus as a master (address for master is optional)

  Serial.begin(9600);  // start serial for output
  while (!Serial) {};

  Serial.println();
  Serial.println("============ START: [JBL XTREAME 2 Emulator] ============");
  Serial.println("License: MIT License");
  Serial.println("Copyright (c) 2024 Oleksii Sylichenko");
  Serial.println("=========================================================");
}

void sendCommand(byte command) {
  Serial.print("sendCommand: 0x");
  printByte(command);
  Serial.print("; ");

  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(command);
  Wire.endTransmission();
}

int requestData() {
  Serial.print("respondData: ");

  byte data[2];
  Wire.requestFrom(DEVICE_ADDR, sizeof(data));
  for (int i = 0; Wire.available() && i < sizeof(data); i++) { // slave may send less than requested
    data[i] = Wire.read();
  }

  printData(data);
  Serial.print(" -> ");
  const word retval = composeWord(data);
  Serial.print(retval);

  return retval;
}

/**
  12.1.5 0x08/09 Voltage()
*/
void requestVoltage() {
  sendCommand(Commands::VOLT);
  float volt = requestData();
  volt = volt / 1000;
  Serial.print(" = ");
  Serial.print(volt, 3); // 0.001V
  Serial.print(" V");
  Serial.println();
}

/**
  12.1.23 0x2C/2D RelativeStateOfCharge()
*/
void requestRsoc() {
  sendCommand(Commands::RSOC);
  int soc = requestData();
  Serial.println(" = " + String(soc) + " %");
}

/**
  12.1.4 0x06/07 Temperature()
*/
void requestTemperature() {
  static const float ABS_ZERO = -273.15;

  sendCommand(Commands::TEMP);
  float temp = requestData();
  temp = temp / 10 + ABS_ZERO;
  Serial.print(" = ");
  Serial.print(temp, 1); // 0.1°
  Serial.print("°C");
  Serial.println();
}

void loop() {
  static long voltRsocTime = millis(),
              tempTime = millis();

  const long voltRsocDelay = (millis() + TIME_OFFSET_VOLT_RSOC_TEMP) - voltRsocTime;
  if (voltRsocDelay < 0 || VOLT_RSOC_DELAY <= voltRsocDelay) {
    voltRsocTime = millis();
    requestVoltage();
    requestRsoc();
    Serial.println();
  }

  const long tempDelay = millis() - tempTime;
  if (tempDelay < 0 || TEMP_DELAY <= tempDelay) {
    tempTime = millis();
    requestTemperature();
    Serial.println();
  }
}
