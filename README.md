# Mini Flac Player

Seiya Ono - Su'18

## Prototyping

I have ordered the [Adafruit VS1053 Breakout](https://www.adafruit.com/product/1381) and an Arduino Uno. This is to make sure that what I can prototype is up to my standards. I chose the Uno because of its detachable ATMegaa. [Here](https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard) is a simple article about programming the ATMega over the interface.

## Power

Because I will only be using a single cell Li-ion battery, I've went and compared a few charger/discharge protection chips, and landed with the BQ2973. It is extremely simple, only requires a few components, but does not give any sort of indication on battery level, which is the only concern I have.

### Datasheets

[VS1053](https://cdn-shop.adafruit.com/datasheets/vs1053.pdf)

[ATmega328P](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf)

[BQ2973](http://www.ti.com/lit/ds/symlink/bq2973.pdf)

[CSD16301Q2](http://www.ti.com/lit/ds/symlink/csd16301q2.pdf)
