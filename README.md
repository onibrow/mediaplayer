# Mini Flac Player

Seiya Ono - Su'18

## Prototyping

I have ordered the [Adafruit VS1053 Breakout](https://www.adafruit.com/product/1381) and an Arduino Uno. This is to make sure that what I can prototype is up to my standards. I chose the Uno because of its detachable ATMegaa. [Here](https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard) is a simple article about programming the ATMega over the interface.

Here is a good library I can test: [mpflaga](https://mpflaga.github.io/Arduino_Library-vs1053_for_SdFat/)

Small article with someone having trouble with `XTEST` pin: <http://www.toughdev.com/content/2013/10/interfacing-vs1053-audio-encoderdecoder-module-with-pic-using-spi/>

## Power

Because I will only be using a single cell Li-ion battery, I've went and compared a few charger/discharge protection chips, and landed with the BQ2973. It is extremely simple, only requires a few components, but does not give any sort of indication on battery level, which is the only concern I have.

Another chip I found is the [BQ24270](http://www.ti.com/product/BQ24270/technicaldocuments) made for exactly my needs.

This one has been deactivated, so I will switch over to the [BQ24297](http://www.ti.com/product/bq24297/description). An arduino library can be found [here](https://github.com/IRNAS/bq2429x).

[New LiPo Charging Circuit](https://learn.adafruit.com/adafruit-microlipo-and-minilipo-battery-chargers/downloads)

### Datasheets

[VS1053](https://cdn-shop.adafruit.com/datasheets/vs1053.pdf)

[ATmega328P](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf)

[BQ2973](http://www.ti.com/lit/ds/symlink/bq2973.pdf)

[CSD16301Q2](http://www.ti.com/lit/ds/symlink/csd16301q2.pdf)

[BQ24297](http://www.ti.com/lit/ds/symlink/bq24297.pdf)

[MCP73831T](https://www.digikey.com/product-detail/en/microchip-technology/MCP73831T-2ACI-OT/MCP73831T-2ACI-OTCT-ND/1979802)

### BOM

Mega Chip mount: <https://www.digikey.com/products/en?keywords=ED3128-ND>

VS1053: <https://www.digikey.com/product-detail/en/adafruit-industries-llc/1681/1528-2645-ND/>

ATMega328: <https://www.digikey.com/product-detail/en/microchip-technology/ATMEGA328PB-AUR/ATMEGA328PB-AURCT-ND/5722706>

Codec Crystal: <https://www.digikey.com/product-detail/en/abracon-llc/ABM8G-12.288MHZ-18-D2Y-T/535-10262-1-ND/2218075>

### Assembly

I need to only do the microcontroller and peripherals first, so I can power it using 5V and flash it with the bootloader (no regulator).
