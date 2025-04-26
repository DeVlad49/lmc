#include <Adafruit_BusIO_Register.h>
#include <Adafruit_GenericDevice.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>

#include "Metronome.h"
#include "Photoresistor.h"
#include <DS3232RTC.h> // https://github.com/JChristensen/DS3232RTC
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <SPI.h>
#include <Adafruit_GFX.h> // https://github.com/adafruit/Adafruit-GFX-Library
#include <Max72xxPanel.h> // https://github.com/markruys/arduino-Max72xxPanel
#include <dht.h> // https://github.com/RobTillaart/DHTlib

// For debugging
#define VERBOSE_PHOTORESISTOR false
#define VERBOSE_BUFFERS true

#define DHT11_PIN 9
#define LED_PIN 8 // D13 is already used by SPI, so choose different OUTPUT pin

#define MODE_SWITCH_DELAY 10000

#define BRIGHTNESS_DROP_PERCENTAGE 20
#define REGULAR_THRESHOLD_UPDATE_PERIOD 20000

// Throbber-related variables
#define THROBBER_INNER_RADIUS 4
#define THROBBER_OUTER_RADIUS 8
#define ANGLE_STEP 30
int angle = 0;

// Display mode variables
byte mode = 0;
byte lastMode = 3;
bool ledIsLit = false;

// Variables for setting up the LED matrix
int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

// Display-related variables
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels
int y = (matrix.height() - 8) / 2; // center the text vertically

// Buffers for C-strings
char timeBuffer[6];
//char secondsBuffer[5];
char dateBuffer[11];
char tempBuffer[6];
char humiBuffer[6];

// Objects
Metronome mtm(MODE_SWITCH_DELAY);
Metronome rtu_mtm(REGULAR_THRESHOLD_UPDATE_PERIOD); // regular threshold update
Photoresistor pr(A0, BRIGHTNESS_DROP_PERCENTAGE);
DS3232RTC rtc;
dht DHT;

// Declaring functions
void updateLightSensorSensitivity(unsigned long period = 1000);

void setup() {
  Serial.begin(1200);
  rtc.begin();

  // Set time on system &  RTC
  // Uncomment two lines below to set time

//   setTime(14, 57, 0, 20, 3, 25); // setTime(int hr,int min,int sec,int day, int month, int yr);
//   rtc.set(now());

  // Synchronise system time with RTC time
  setSyncProvider(rtc.get);   // the function to get the time from the RTC
  if(timeStatus() != timeSet){
      Serial.println("Unable to sync with the RTC");
  }else{
      Serial.println("RTC has set the system time");
  }

  // When matrix connections are on the left
  for (byte i = 0; i<5; i++){
    matrix.setRotation(i, 1);
  }

  // Setting a threshold for photoresistor
  updateLightSensorSensitivity();

  // LED indicator
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {

  // Store formatted time and date strings
  time_t t = now();
  sprintf(timeBuffer, "%02d:%02d", hour(t), minute(t));
//    sprintf(secondsBuffer, "%02d s", second(t));
  sprintf(dateBuffer, "%02d.%02d.%04d", day(t), month(t), year(t));

  // Store formatted temperature and humidity
  int chk = DHT.read11(DHT11_PIN);
  sprintf(tempBuffer, "t:%dC", int(DHT.temperature));
  sprintf(humiBuffer, "h:%d%%", int(DHT.humidity));

  // Display time on LED matrix
  // Checking if the time passed.
  // If it did, then change display mode
  if (mtm.intervalPassed()){
    mode++;
//    Serial.println(mode);
  }

  // Checking if photoresistor is covered

  // Regularly update average environmental brightness value
  if (rtu_mtm.intervalPassed()){
    updateLightSensorSensitivity();
    rtu_mtm.reset();
  }

  if (pr.isCoveredInLessThanTimePeriod(700, VERBOSE_PHOTORESISTOR)){
    Serial.println("CHANGING DISPLAY MODE");
    mode++;
    mtm.reset();

    // Changing LED state
    if (!ledIsLit){
      ledIsLit = true;
      digitalWrite(LED_PIN, HIGH);
    }
    else{
      ledIsLit = false;
      digitalWrite(LED_PIN, LOW);
    }
  }

  // Switching display modes

  if(mode>3){
    mode = 0;
  }

  if (mode != lastMode){
    matrix.fillScreen(LOW);
    switch(mode){
      case 0:
        Serial.println(timeBuffer);
        displayCentredText(String(timeBuffer));
        break;
      case 1:
        Serial.println(dateBuffer);
        displayCentredText(String(dateBuffer));
        break;
      case 2:
        Serial.println(tempBuffer);
        displayCentredText(String(tempBuffer));
        break;
      case 3:
        Serial.println(humiBuffer);
        displayCentredText(String(humiBuffer));
        break;
      default:
        Serial.println("Err");
        displayCentredText(String("Err"));
        break;
    }
    lastMode = mode;
  }
  
  // Send bitmap to display
  matrix.write();
}

void displayCentredText(String str){
  int strLen = str.length();
  int clockWidth = width * strLen;
  int x = (matrix.width() - clockWidth) / 2;

  byte digit  = 0;

  while (digit < strLen){
    matrix.drawChar(x, y, str[digit], HIGH, LOW, 1);
    x += width;
    digit++;
  }
}

void updateLightSensorSensitivity(unsigned long period = 1000){
  Serial.println("Updating environmental brightness... ");
  
  unsigned long startTime = millis();
  long sum = 0;
  int sampleCount = 0;

  while (millis() - startTime < period) {
    sum += pr.read();
    sampleCount++;

    drawThrobber(THROBBER_INNER_RADIUS, THROBBER_OUTER_RADIUS, angle);
    angle = (angle + ANGLE_STEP) % 360;
    
    delay(50);
  }
  pr.setEnvironmentalBrightness(int(sum / sampleCount), VERBOSE_PHOTORESISTOR);
  pr.updateThreshold(VERBOSE_PHOTORESISTOR);
  Serial.println("Done!");
}

void drawThrobber(int innerRadius, int outerRadius, int degrees){
  int centerX = matrix.width() / 2;
  int centerY = matrix.height() / 2 - 1;

  float radians = degrees * DEG_TO_RAD;
  
  int innerX = innerRadius * cos(radians);
  int innerY =  innerRadius * sin(radians);
  
  int outerX = outerRadius * cos(radians);
  int outerY =  outerRadius * sin(radians);
  
  matrix.fillScreen(LOW);
  matrix.writeLine(centerX + innerX, centerY + innerY, 
                   centerX + outerX, centerY + outerY, 1);
  matrix.write();
}
