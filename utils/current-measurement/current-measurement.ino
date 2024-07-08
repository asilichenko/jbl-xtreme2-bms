/**
  @file current_measurement.ino

  This utility program helps calibrate constants for accurate current measurement and calculation.

  Due to the voltage used by the JBL XTREME 2 being much greater than what can be measured
  by the Arduino, a voltage divider should be used.

  There are two voltage dividers: one for each end of the current sense shunt.
  Connect the common ends of the dividers together and ground them.
  The midpoint of each divider should be connected to the corresponding Arduino pin.
  The remaining ends of the dividers should be connected to the ends of the current sense shunt.


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

#define PIN_VOLT_1 A6
#define PIN_VOLT_2 A7

#define ONE_MINUTE 60000
#define FIVE_SECONDS 5000

/**
  @brief Structure to hold raw ADC values for current measurement using a shunt resistor.

  This structure stores the raw ADC values obtained from the voltage divider connected
  to the input voltage, the calculated raw ADC value at the shunt resistor's input point,
  and the calculated raw ADC voltage drop across the shunt resistor.
*/
struct CurrentMeasurementData {
  float adcValueDivider1;   ///< ADC Value Divider 1
  float adcValueDivider2;   ///< ADC Value Divider 2
  float adcValueShunt1;     ///< ADC Value Shunt 1
  float adcValueShunt2;     ///< ADC Value Shunt 2
  float adcValueShuntDrop;  ///< ADC Value Shunt Drop
};

enum Mode {
  PRINT_ANALOG_VALUES,
  MEASURE
};

const Mode MODE = Mode::MEASURE;

/**
  @brief Arduino Reference voltage.

  By default: VCC = 5V
*/
const int REF_VOLT = 5029;

const float VOLTAGE_DIVIDER_RATIO_1 = 7579.0 / 3781;  ///< Ratio of the voltage divider 1
const float VOLTAGE_DIVIDER_RATIO_2 = 7529.0 / 3756;  ///< Ratio of the voltage divider 2

const int R_SHUNT = 388;  ///< Resistance of the current-sense (shunt) resistor, in milliohms.

/**
  @brief Value that is used to convert ADC readings to voltage.
  @see "23.7 ADC Conversion Result" https://content.arduino.cc/assets/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
*/
const int ADC_CONVERSION_VALUE = 1024;

void setup() {
  Serial.begin(9600);  // start serial for output
  while (!Serial) {};  // wait until serial is ready

  Serial.println();
  Serial.println("============ START: [Current Measurement] ============");
  Serial.println("License: MIT License");
  Serial.println("Copyright (c) 2024 Oleksii Sylichenko");
  Serial.println("======================================================");
  Serial.println();

  pinMode(PIN_VOLT_1, INPUT);
  pinMode(PIN_VOLT_2, INPUT);
}

/**
  @brief Print the analog voltage readings across the shunt.

  Print 1000 pairs of analog readings: 100 pairs in 10 rows with extra empty line.
*/
void printAnalogValues() {
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < 100; i++) {
      const int analogValue1 = analogRead(PIN_VOLT_1);
      const int analogValue2 = analogRead(PIN_VOLT_2);
      Serial.print(String(analogValue1) + "," + String(analogValue2) + ";");
    }
    Serial.println();
  }
  Serial.println();
}

/**
  Obtain the raw ADC values from the voltage divider connected
  to the input voltage, the calculated raw ADC value at the shunt resistor's input point,
  and the calculated raw ADC voltage drop across the shunt resistor.

  @returns Structure that holds raw ADC values of current measurement using a shunt resistor.
*/
CurrentMeasurementData getCurrentMeasurementData() {
  long dividerSum1 = 0, dividerSum2 = 0;
  static const int READINGS_NUMBER = 1000;
  for (int i = 0; i < READINGS_NUMBER; i++) {
    dividerSum1 += analogRead(PIN_VOLT_1);
    dividerSum2 += analogRead(PIN_VOLT_2);
  }

  const float adcValueDivider1 = 1.0 * dividerSum1 / READINGS_NUMBER;
  const float adcValueDivider2 = 1.0 * dividerSum2 / READINGS_NUMBER;

  const float adcValueShunt1 = VOLTAGE_DIVIDER_RATIO_1 * adcValueDivider1;
  const float adcValueShunt2 = VOLTAGE_DIVIDER_RATIO_2 * adcValueDivider2;

  CurrentMeasurementData retval;
  retval.adcValueDivider1 = adcValueDivider1;
  retval.adcValueDivider2 = adcValueDivider2;
  retval.adcValueShunt1 = adcValueShunt1;
  retval.adcValueShunt2 = adcValueShunt2;
  retval.adcValueShuntDrop = retval.adcValueShunt1 - retval.adcValueShunt2;

  return retval;
}

int adc2volt(float analogValue) {
  return round(analogValue * REF_VOLT / ADC_CONVERSION_VALUE);
}

void currentMeasurement() {
  const CurrentMeasurementData data = getCurrentMeasurementData();
  const int vDivider1 = adc2volt(data.adcValueDivider1);
  const int vDivider2 = adc2volt(data.adcValueDivider2);
  const int vShunt1 = adc2volt(data.adcValueShunt1);
  const int vShunt2 = adc2volt(data.adcValueShunt2);
  const int vShuntDrop = adc2volt(data.adcValueShuntDrop);
  const int current = 1000.0 * vShuntDrop / R_SHUNT;

  Serial.println(
    "adcValueDivider1:" + String(data.adcValueDivider1)
    + ";adcValueDivider2:" + String(data.adcValueDivider2)
    + ";vDivider1(mV):" + String(vDivider1)
    + ";vDivider2(mV):" + String(vDivider2)
    + ";adcValueShunt1:" + String(data.adcValueShunt1)
    + ";adcValueShunt2:" + String(data.adcValueShunt2)
    + ";vShunt1(mV):" + String(vShunt1)
    + ";vShunt2(mV):" + String(vShunt2)
    + ";vShuntDrop(mV):" + String(vShuntDrop)
    + ";current(mA):" + String(current)
  );
}

void loop() {
  if (Mode::PRINT_ANALOG_VALUES == MODE) {
    printAnalogValues();
    delay(ONE_MINUTE);
  } else if (Mode::MEASURE == MODE) {
    currentMeasurement();
    delay(FIVE_SECONDS);
  }
}
