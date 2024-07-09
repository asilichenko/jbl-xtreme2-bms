# Temperature Measurement Util

This utility program helps calibrate constants for accurate temperature measurement and calculation.

## Equipment
- Thermistor, NTC, 10kOhm.
- Resistor, 10kOhm

## Connections
- Connect thermistor between VCC and Arduino analog pin.
- Connect series resistor between Ground and thermistor/analog pin.

<img width="300" src="/utils/img/thermistor_connection.png" alt="Thermistor connection"/>

> [!IMPORTANT]
> If the thermistor is connected to the GND then use "analogValue = 1024 - analogValue" instead.

## Author
[@asilichenko](https://github.com/asilichenko)

## License
[MIT license](/LICENSE)

## See also
- [Calibrating a Thermistor for Arduino Projects](https://www.linkedin.com/pulse/step-by-step-guide-calibrating-thermistor-arduino-oleksii-sylichenko-yvwif)

## Sources
- [temperature-measurement.ino](utils/temperature-measurement/temperature-measurement.ino)
