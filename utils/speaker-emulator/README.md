# JBL XTREME 2 Emulator Util

This program periodically sends commands to the battery pack gas gauging device as the real speaker does.

<img width="600" src="speaker-emulator.png" alt="JBL XTREME 2 Emulator"/>

The commands sent are standard commands for the gas gauging device BQ28Z610:
- 12.1.4 0x06/07 Temperature()
- 12.1.5 0x08/09 Voltage()
- 12.1.23 0x2C/2D RelativeStateOfCharge()

Commands Voltage() and RelativeStateOfCharge() are sent in pair once per 5 seconds.

Command Temperature() is sent ones per 10s.

There is can be any time shift between them.

## Connection

Arduino Nano:
- A4 = SDA -> white
- A5 = SCL -> yellow
- GND = GND -> black

## Author
[@asilichenko](https://github.com/asilichenko)

## License
[MIT license](/LICENSE)

## References
- Texas Instruments Technical Reference Manual https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
- BQ28Z610 product: https://www.ti.com/product/BQ28Z610
- [Arduino Stub for Battery Pack](/battery-stub)
- [Arduino Gas Gauging](gas-gauging)

## Sources
[speaker-emulator.ino](speaker-emulator.ino)
