#include "Metronome.h"
#include <DS3232RTC.h> // https://github.com/JChristensen/DS3232RTC
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

byte mode = 0;

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

// Objects
Metronome mtm(10000);
DS3232RTC rtc;

void setup() {
  Serial.begin(1200);
  rtc.begin();

  // Set time on system &  RTC
  // Uncomment two lines below to set time

//   setTime(23, 0, 25, 3, 3, 2025); // setTime(int hr,int min,int sec,int day, int month, int yr);
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
    sprintf(dateBuffer, "%02d.%02d.%04d", day(t), month(t), year(1));

    // Print time to Serial
//    Serial.println(timeAndDateBuffer);
//    Serial.println(timeBuffer);
//    Serial.println(secondsBuffer);
//    Serial.println(dateBuffer);

    // Display time on LED matrix
    matrix.fillScreen(LOW);
    displayCentredText(String(timeBuffer));
    matrix.write(); // Send bitmap to display

  bool passed = mtm.intervalPassed();
  if (passed){
    mode++;
    if(mode>1){
      mode = 0;
    }
  }
  switch(mode){
    case 0:
      Serial.println(timeBuffer);
      break;
    case 1:
      Serial.println(dateBuffer);
      break;
    default:
      Serial.println("Err");
      break;
  }
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
