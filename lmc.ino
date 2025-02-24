#include "Metronome.h"
#include <DS3232RTC.h>

// Objects
Metronome mtm(1000);
DS3232RTC myRTC;

// Variables
int ledState = LOW;

void setup() {
  Serial.begin(300);
  myRTC.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  delay(5000);

// Set RTC time
// setTime(23, 31, 30, 13, 2, 2009);   // set the system time to 23h31m30s on 13Feb2009

  setTime(15, 2, 0, 24, 2, 2025);
//  myRTC.set(now());                   // set the RTC from the system time

  // Synchronise system time with RTC time
  setSyncProvider(myRTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");
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
    time_t t = now();
    Serial.print(hour(t));
    Serial.print(':');
    Serial.print(minute(t));
    Serial.print(':');
    Serial.println(second(t));
  }
}
