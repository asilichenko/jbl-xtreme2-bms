/**
  @file BQ28Z610-stub.ino

  @brief This is Gas Gauging Device BQ28Z610 Stub for JBL XTREAME 2 on Arduino.

  This stub can be used when you don't want to or cannot use the battery pack.

  Write it on Arduino or ATtiny, connect it to the JBL speaker,
  and now the speaker can work mains-powered without the battery.

  Important: The stub should have its own power supply, at least a supercapacitor,
  because the JBL speaker doesn't supply power until it receives the correct response
  to the I2C commands.

  @warning [!] When you use this stub, the real battery should not be connected to the speaker,
  because the stub does not track the state of the battery; it just responds with hardcoded values.

  @see Texas Instruments Technical Reference Manual https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
  @see BQ28Z610 product: https://www.ti.com/product/BQ28Z610

  Connections

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
#include <avr/sleep.h>

#define adc_disable() bitClear(ADCSRA, ADEN)

/**
  @brief When is defined - messages will be printed into Serial port.
*/
//#define DEBUG

/**
  @brief Standard BQ28Z610 Device I2C Address
  @see 12.1 Standard Data Commands
*/
const byte DEVICE_ADDR = 0x55;

/**
  @brief Pack voltage in millivolts.

  Can be ignored.

  However, if the battery is connected, then while the battery is being used,
  the level of charge indicator will always display the last red blinking dot,
  but the speaker will continue to work.
*/
const word VOLT = 8400;  // 8.4V
/**
  @brief Battery temperature in units of 0.1 Kelvin.
  @warning [!] Must mandatory: if too high or too low - the speaker turns off.
*/
const word TEMP = 3000;  // 26.85°C: (26.85 + 273.15) * 10 = 3000
/**
  @brief Relative state of battery charge.

  When the Speacker is mains powered - the last active dot is always blinking.

  - >= 85 - 5 dots
  - [70-84] - 4 dots
  - [55-69] - 3 dots
  - [40-54] - 2 dots
  - [18-39] - 1 dot
  - <= 17 - blinking red dot
  - < 10 - turns off after some time

  @warning [!] Must mandatory: if 0 - you cannot play audio even when charging,
  speacker turns off and continues charging.
*/
const word RSOC = 100;  // 100%

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

void setup() {
  Wire.begin(DEVICE_ADDR);      // join i2c bus with address DEVICE_ADDR
  Wire.onReceive(receiveEvent); // register incoming command event
  Wire.onRequest(requestEvent); // register request to answer event

  adc_disable();

#ifdef DEBUG
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\n============ START: [BQ28Z610 Stub for JBL XTREAME 2 on Arduino] ============\nLicense: MIT License\nCopyright (c) 2024 Oleksii Sylichenko\n==================================================\n");
  delay(100);  // wait to finish printing
#endif
}

/**
  @brief Print "Command: 0xAB\n"
*/
void printCommand(byte command) {
  Serial.print("Command: 0x");
  if (command < 0xF) Serial.print('0');
  Serial.println(command, HEX);
  delay(10);  // wait to finish printing
}

/**
  @brief Wether the speacker is active.
*/
volatile bool isActive = false;

/**
  @brief Received command.
*/
volatile byte command = 0;

void receiveEvent() {
  isActive = true;
  command = Wire.read();

#ifdef DEBUG
  printCommand(command);
#endif
}

/**
  @brief Print "Response: 0xABCD\n"
*/
void printResponse(word response) {
  Serial.print("Response: 0x");
  if (response < 0xFFF) Serial.print('0');
  if (response < 0xFF) Serial.print('0');
  if (response < 0xF) Serial.print('0');
  Serial.println(response, HEX);
  Serial.println();
  delay(10);  // wait to finish printing
}

/**
  @brief Write the word value back to the I2C bus in little-endian format.
*/
void writeResponse(word value) {
  const byte LBS = value & 0xFF;
  const byte MSB = (value >> 8) & 0xFF;
  const byte data[] = {LBS, MSB};
  Wire.write(data, sizeof(data));
}

/**
  @brief Executes whenever data is requested by master.
  This function is registered as an event.
  @see setup()
*/
void requestEvent() {
  word value;
  bool isUnsupportedCommand = false;
  if (Commands::TEMP == command) value = TEMP;  // Temperature in units 0.1°K (273.15 = 2731)
  else if (Commands::VOLT == command) value = VOLT;  // Pack Voltage in millivolts
  else if (Commands::RSOC == command) value = RSOC;  // Predicted remaining battery capacity as a percentage
  else isUnsupportedCommand = true;
  command = 0;

  if (isUnsupportedCommand) return;

#ifdef DEBUG
  printResponse(value);
#endif

  writeResponse(value);
}

void goSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();       // Set the SE (sleep enable) bit.
  sleep_bod_disable();  // Recommended practice is to disable the BOD directly before sleep
  sleep_cpu();          // Put the device into sleep mode. The SE bit must be set beforehand, and it is recommended to clear it afterwards.

  // --- wake up ---
  sleep_disable();      // Clear SE bit after wake up
}

void loop() {
  isActive = false;
  delay(30000);  // 30s
  if (!isActive) goSleep();
}
