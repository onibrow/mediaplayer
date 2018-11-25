# Mini Flac Player

Seiya Ono - Su'18

## Prototyping

I have ordered the [Adafruit VS1053 Breakout](https://www.adafruit.com/product/1381) and an Arduino Uno. This is to make sure that what I can prototype is up to my standards. I chose the Uno because of its detachable ATMegaa. [Here](https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard) is a simple article about programming the ATMega over the interface.

Here is a good library I can test: [mpflaga](https://mpflaga.github.io/Arduino_Library-vs1053_for_SdFat/)

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

### BOM

Mega Chip mount: <https://www.digikey.com/products/en?keywords=ED3128-ND>
