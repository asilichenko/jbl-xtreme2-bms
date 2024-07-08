/**
  @file temperature_measurement.ino

  This utility program helps calibrate constants for accurate temperature measurement and calculation.

  Used equipment:
  - Thermistor, NTC, 10kOhm.
  - Resistor, 10kOhm

  Connections:
  - Connect thermistor between VCC and Arduino analog pin.
  - Connect series resistor between Ground and thermistor/analog pin.

  Important:
  - if the thermistor is connected to the GND then use "analogValue = 1024 - analogValue" instead.

  @see "Calibrating a Thermistor for Arduino Projects"
  https://www.linkedin.com/pulse/step-by-step-guide-calibrating-thermistor-arduino-oleksii-sylichenko-yvwif


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

#define THERMISTOR_PIN A1

/**
  @brief The Arduino log() function returns the natural logarithm.
*/
#define ln log

#define FIVE_SECONDS 5000

const int BETA = 3550;                ///< B-value of the thermistor.
const int SERIES_RESISTANCE = 10300;  ///< Resistanse of the serial resistor in ohms.

/**
  @brief Value that is used to convert ADC readings to voltage.
  @see "23.7 ADC Conversion Result" https://content.arduino.cc/assets/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
*/
const int ADC_CONVERSION_VALUE = 1024;

void setup() {
  Serial.begin(9600);  // start serial for output
  while (!Serial) {};  // wait until serial is ready

  Serial.println();
  Serial.println("============ START: [Temperature Measurement] ============");
  Serial.println("License: MIT License");
  Serial.println("Copyright (c) 2024 Oleksii Sylichenko");
  Serial.println("==========================================================");
  Serial.println();

  pinMode(THERMISTOR_PIN, INPUT);
}

/**
  @brief Make 1000 analog readings and calculate the average.
  @return average of 1000 readings.
*/
float getAnalogValueAvg() {
  long sum = 0;
  static const int READINGS_NUMBER = 1000;
  for (int i = 0; i < READINGS_NUMBER; i++) sum += analogRead(THERMISTOR_PIN);
  return 1.0 * sum / READINGS_NUMBER;
}

/**
  @brief Standard Steinhart–Hart Equation.
  @param resistance resistance of the thermistor.
  @returns temperature of the thermistor in Celsius degrees.
*/
float computeTemperature(float resistance) {
  static const int NOMINAL_RESISTANCE = 10000;  // The nominal resistance of the thermistor at 25°C, in ohms
  static const int NOMINAL_TEMPERATURE = 25;    // The temperature at which the resistance of the thermistor is equal to its nominal value, in Celsius.
  static const float ABS_ZERO = -273.15;        // Difference between the Celsius and Kelvin scales.

  float retval;
  retval = resistance / NOMINAL_RESISTANCE;  // R/Ro
  retval = ln(retval);  // ln(R/Ro)
  retval /= BETA;  // 1/B * ln(R/Ro)
  retval += 1.0 / (NOMINAL_TEMPERATURE - ABS_ZERO);  // + (1/To)
  retval = 1.0 / retval;  // Invert
  retval += ABS_ZERO;  // from Kelvin to Celsius

  return retval;
}

/**
  Thermistor should be connected between Arduino analog pin and VCC.

  Otherwise use "alalogValue = 1024 - analogValue"

  @param analogValue ADC value on the thermistor pin.
  @returns resistance of the thermistor in ohms.
*/
float computeResistance(float analogValue) {
  return (ADC_CONVERSION_VALUE / analogValue - 1.0) * SERIES_RESISTANCE;
}

void loop() {
  const float analogValue = getAnalogValueAvg();
  const float thermistorResistance = computeResistance(analogValue);
  const float temperature = computeTemperature(thermistorResistance);

  Serial.print(temperature, 1);
  Serial.println(" °C");

  delay(FIVE_SECONDS);
}
