# bozontlabsMAX7219

MAX7219 Arduino driver library for LED matrix displays.

The library supports both software emulated and dedicated hardware SPI interfaces for driving displays. Software emulated SPI is great for flexibility - the display pins can be mapped to any GPIO. Hardware SPI can only be used on the dedicated SPI pins, however it'll be multiple times faster in driving the display than the software emulated bus.

## Features
 - Software emulated SPI support
 - Hardware SPI support
 - Absolute addressing of pixels
 - Brightness control
 - Power control

##### This library is originally based on the [LedControl](https://github.com/wayoda/LedControl) library and includes the license from there.
