#include "Metronome.h"

Metronome mtm(1000);
int ledState = LOW;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  delay(5000);
}

void loop() {
  bool passed = mtm.intervalPassed();
  Serial.println(passed);
  if (passed){
    if (ledState == LOW){
      ledState = HIGH;
    }else{
      ledState = LOW;
    }
    digitalWrite(LED_BUILTIN, ledState);
  }
}
