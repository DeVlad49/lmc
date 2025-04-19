#include "Metronome.h"
#include <DS3232RTC.h> // https://github.com/JChristensen/DS3232RTC
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <SPI.h>
#include <Adafruit_GFX.h> // https://github.com/adafruit/Adafruit-GFX-Library
#include <Max72xxPanel.h> // https://github.com/markruys/arduino-Max72xxPanel
#include <dht.h> // https://github.com/RobTillaart/DHTlib

#define DHT11_PIN 9
#define PIR_PIN 8
//#define POWER_DPIN 7 // For Uno only
#define MODE_SWITCH_DELAY 10000

byte mode = 0;
byte lastMode = 3;
int pirState = LOW;
int pirValue = 0;

// Variables for setting up the LED matrix
int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

// Display-related variables
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels
int y = (matrix.height() - 8) / 2; // center the text vertically

int wait = 40; // In milliseconds

// Buffers for C-strings
char timeBuffer[6];
//char secondsBuffer[5];
char dateBuffer[11];
char tempBuffer[6];
char humiBuffer[6];

// Objects
Metronome mtm(MODE_SWITCH_DELAY);
DS3232RTC rtc;
dht DHT;

void setup() {
  Serial.begin(1200);
  rtc.begin();

  pinMode(PIR_PIN, INPUT);
//  pinMode(POWER_DPIN, OUTPUT); // For Uno only
//  digitalWrite(POWER_DPIN, HIGH); // For Uno only

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

  // When connections are on the left
  for (byte i = 0; i<5; i++){
    matrix.setRotation(i, 1);
  }
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
  bool passed = mtm.intervalPassed();
  if (passed){
    mode++;
//    Serial.println(mode);
  }

  // Checking if PIR is active
  pirValue = digitalRead(PIR_PIN);
  if (pirValue == HIGH){
    if (pirState == LOW){
      Serial.println("Motion detected!");
      pirState = HIGH;
      mode++;
      mtm.reset();
    }
  }else{
    if (pirState == HIGH){
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }

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
