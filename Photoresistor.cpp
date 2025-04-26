#include "Photoresistor.h"

Photoresistor::Photoresistor(int analogPin, byte dropPercent) {
  m_pin = analogPin;
  m_brightnessDrop = float(dropPercent) / 100.0;
}

void Photoresistor::setEnvironmentalBrightness(int brightness, bool verbose){
  m_environmentalBrightness = brightness;
  if (verbose){
    Serial.print("UPDATED ENVIRONMENTAL BRIGHTNESS: ");
    Serial.println(m_environmentalBrightness);
  }
}

int Photoresistor::getEnvironmentalBrightness(){
  return m_environmentalBrightness;
}

void Photoresistor::updateEnvironmentalBrightness(unsigned long measurementTime) {
  long sum = 0;
  unsigned long startTime = millis();
  int sampleCount = 0;

  while (millis() - startTime < measurementTime) {
    sum += analogRead(m_pin);
    sampleCount++;
    delay(50);
  }

  m_environmentalBrightness = sum / sampleCount;
}

int Photoresistor::getThreshold(){
  return m_threshold;
}

void Photoresistor::updateThreshold(bool verbose) {
  m_threshold = int(float (m_environmentalBrightness) * (1 - m_brightnessDrop));
  if (verbose){
    Serial.print("UPDATED THRESHOLD: ");
    Serial.println(m_threshold);
  }
  
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
    if (verbose){
      Serial.print("Covered: ");
      Serial.println(read());
    }
  }
  else if (isDark() && m_covered){
    if (verbose){
      Serial.print("Covered: ");
      Serial.print(read());
      Serial.println(" ; Won't react.");
    }
  }
  else if (!isDark() && m_covered){
    m_covered = false;
    m_coverEnd = millis();
    if (verbose) Serial.print("It is bright again: ");
    Serial.println(read());
  
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
    if (verbose){
      Serial.print("Uncovered: ");
      Serial.print(read());
      Serial.println(" ; Won't react");
    }
  }
  return false;
}
