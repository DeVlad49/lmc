#include "Metronome.h"

Metronome::Metronome(unsigned long interval):
  m_interval(interval),
  m_lastMillis(0)
  {
//    Serial.begin(300);
  }

bool Metronome::intervalPassed(){
  unsigned long newMillis = millis();
  unsigned long diff = newMillis - m_lastMillis;
//  Serial.print("Diff: ");
//  Serial.println(diff);
  if (diff >= m_interval){
    m_lastMillis = newMillis;
    return true;
  }else{
    return false;
  }
}

void Metronome::reset(){
  m_lastMillis = millis();;
}

unsigned long Metronome::getLastMillis(){
  return m_lastMillis;
}

unsigned long Metronome::getPassedTime(){
  return millis() - m_lastMillis;
}
