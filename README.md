# JBL XTREME 2 BMS
Arduino sketches to analyse and emulate BMS microchip of the Battery Pack for the JBL XTREME 2 Bluetooth Speaker.

## Module 1. Stub for Battery Pack

This program is designed to be written on Arduino or ATtiny85 and acts as a stub to respond to the speaker's requests via the I2C protocol, enabling the speaker to be used while connected to mains power without using a battery.

It can be used when you don't want to or cannot use the battery pack.

<img width="300" src="battery-stub/img/stub-active-speaker-220-on-switch-on.png" alt="JBL XTREME 2 Battery Stub"/>

[Explore->](battery-stub)

## Utils

- [Address of the battery device](#address-of-the-battery-device)
- [Commands receiver](#commands-receiver)
- [Speaker emulator](#speaker-emulator)
- [Temperature measurement](#temperature-measurement)
- [Current measurement](#current-measurement)

### Address of the battery device

To find out the I2C address of the battery gas gauging device you may use the sketch from the examples:

<img height="600" src="utils/i2c_scanner.png" alt="Arduino I2C device scanner"/>

### Commands receiver

You may use this program to log commands from the speaker.

<img width="450" src="utils/commands-receiver/commands-receiver.png" alt="JBL XTREAME 2 Commands Receiver"/>

[Explore->](utils/commands-receiver)

### Speaker emulator

This util periodically sends commands to the battery pack gas gauging device as the real speaker does.

<img width="450" src="utils/speaker-emulator/speaker-emulator.png" alt="JBL XTREAME 2 Commands Emulator"/>

[Explore->](utils/speaker-emulator)

### Temperature measurement

This utility program helps calibrate constants for accurate temperature measurement and calculation.

<img width="300" src="utils/temperature-measurement/thermistor_connection.png"/>

[Explore->](utils/temperature-measurement)

### Current measurement

This utility program helps calibrate constants for accurate current measurement and calculation.

<img width="300" src="utils/current-measurement/current_measurement.png"/>

[Explore->](utils/current-measurement)

## Author
[@asilichenko](https://github.com/asilichenko)

## License
[MIT license](LICENSE)

## References
- Texas Instruments BQ28Z610 Technical Reference Manual: [https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf](https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf)

## Sources
1. Battery stub: [battery-stub.ino](battery-stub/battery-stub.ino)
2. Utils:
   - [commands-receiver.ino](utils/commands-receiver/commands-receiver.ino)
   - [speaker-emulator.ino](utils/speaker-emulator/speaker-emulator.ino)
   - [temperature-measurement.ino](utils/temperature-measurement/temperature-measurement.ino)
   - [current-measurement.ino](utils/current-measurement/current-measurement.ino)
