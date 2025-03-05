#include "Metronome.h"
#include <DS3232RTC.h> // https://github.com/JChristensen/DS3232RTC
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

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

// Variables for blinking without delay
int ledState = LOW;

// Buffers for C-strings
char buffer[50]; // Буфер для зберігання рядка

// Objects
Metronome mtm(1000);
DS3232RTC rtc;

void setup() {
  Serial.begin(300);
  rtc.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  // Set time on system &  RTC
  // Uncomment two lines below to set time

//   setTime(23, 0, 25, 3, 3, 2025); // setTime(int hr,int min,int sec,int day, int month, int yr);
//   rtc.set(now());

  // Synchronise system time with RTC time
  setSyncProvider(rtc.get);   // the function to get the time from the RTC

  if(timeStatus() != timeSet)
      Serial.println("Unable to sync with the RTC");
  else
      Serial.println("RTC has set the system time");

  // When connections are on the left
  for (byte i = 0; i<5; i++){
    matrix.setRotation(i, 1);
  }

}

void loop() {
  bool passed = mtm.intervalPassed();
  if (passed){

    // Change LED state
    if (ledState == LOW){
      ledState = HIGH;
    }else{
      ledState = LOW;
    }
    digitalWrite(LED_BUILTIN, ledState);

    // Print time to Serial
    time_t t = rtc.get();
    sprintf(buffer, "Time on RTC: %02d:%02d:%02d %04d", hour(t), minute(t), second(t), year(t));
    Serial.println(buffer);

    // Display time on LED matrix
    matrix.fillScreen(LOW);
    displayText(String(buffer));
    matrix.write(); // Send bitmap to display
  }
}

void displayText(String str){
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
