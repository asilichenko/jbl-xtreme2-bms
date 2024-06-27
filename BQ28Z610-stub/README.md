# Gas Gauging Device BQ28Z610 Stub for JBL XTREAME 2 on Arduino

This stub can be used when you don't want to or cannot use the battery pack.
  
Write it on Arduino or ATtiny, connect it to the JBL speaker, and now the speaker can work mains-powered without the battery.

Important: The stub should have its own power supply, at least a supercapacitor, 
because the JBL speaker doesn't supply power until it receives the correct response
to the I2C commands.

## Author
[@asilichenko](https://github.com/asilichenko)

## License
[MIT license](/LICENSE)

## References
- Texas Instruments BQ28Z610 Technical Reference Manual: [https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf](https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf)

## Sources
[BQ28Z610-stub.ino](BQ28Z610-stub.ino)
