# LED Matrix Clock project

## Components:

* 4 LED matrix 1088AS with MAX7219 driver

  The 4 LED matrixes are arranged in a single row. So the display is 8 px high and 8*4=32 px wide.

* RTC DS3231
* TEMT6000 Ambient Light Sensor
* temperature and humidity sensor DHT11

## External libraries

### For LED matrix

* [Max72xxPanel](https://github.com/markruys/arduino-Max72xxPanel)
* [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) (used in the aforementioned library)
* [DHTlib](https://github.com/RobTillaart/DHTlib)
* [RTClib](https://github.com/adafruit/RTClib)
* [Adafruit Bus IO](https://github.com/adafruit/Adafruit_BusIO) (used by RTClib; not installed in some versions of Arduino IDE)
