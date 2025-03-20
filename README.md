# LED Matrix Clock project

## Project notes
See [here](https://docs.google.com/document/d/1wKCPI0wZZf8HCPCKJa1tOnIjr6q0EguBgRXOC0Ul9gY/edit?usp=sharing).

## Components:
* 4 LED matrix 1088AS (with MAX7219 driver)

  The 4 LED matrixes are arranged in a single row. So the display is 8 px high and 8*4=32 px wide.

* RTC DS3231
* PIR sensor (put model name here)
* temperature and humidity sensor DHT11

## External libraries:
### For LED matrix:
* [Max72xxPanel](https://github.com/markruys/arduino-Max72xxPanel)
* [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) (used in previous library)
* [Adafruit Bus IO](https://github.com/adafruit/Adafruit_BusIO) (for some versions of Arduino IDE)
