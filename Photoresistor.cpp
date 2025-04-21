#include "Photoresistor.h"

Photoresistor::Photoresistor(int analogPin, byte dropPercent) {
  m_pin = analogPin;
  m_brightnessDrop = float(dropPercent) / 100.0;
}

int Photoresistor::updateEnvironmentalBrightness(unsigned long measurementTime) {
  long sum = 0;
  unsigned long startTime = millis();
  int sampleCount = 0;

  while (millis() - startTime < measurementTime) {
    sum += analogRead(m_pin);
    sampleCount++;
    // here you can indicate the process
    delay(50);
    // Here you can turn LED off
  }

  return sum / sampleCount;
}

void Photoresistor::updateThreshold(unsigned long measurementTime) {
  Serial.println("Updating environmental brightness... ");
  int brightness = updateEnvironmentalBrightness(measurementTime);
  m_threshold = int(float (brightness) * (1 - m_brightnessDrop));
  Serial.println("Done!");
}

int Photoresistor::read() {
  return analogRead(m_pin);
}

bool Photoresistor::isDark() {
  int current = read();
  return current < m_threshold;
}

bool Photoresistor::isCoveredInLessThanTimePeriod(unsigned long timePeriod, bool verbose) {
  if (isDark() && !m_covered){
    m_covered = true;
    m_coverStart = millis();
    if (verbose) Serial.println("Covered!");
  }
  else if (isDark() && m_covered){
    if (verbose) Serial.println("Covered. Won't react.");
  }
  else if (!isDark() && m_covered){
    m_covered = false;
    m_coverEnd = millis();
    if (verbose) Serial.println("It is bright again! ");
  
    if (m_coverEnd - m_coverStart < timePeriod){
      if (verbose){
        Serial.print("Less than ");
        Serial.print(timePeriod);
        Serial.println(" ms passed!");
        Serial.println("Uncovered! ");
      }
      return true;
    }
    else{
      if (verbose){
        Serial.print("More than ");
        Serial.print(timePeriod);
        Serial.println(" ms passed.");
        Serial.println("Ignoring");
      }
    }
  }
  else if (!isDark() && !m_covered){
    if (verbose) Serial.println("Uncovered. Won't react");
  }
  return false;
}
