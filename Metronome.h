#ifndef MY_METRONOME_H
#define MY_METRONOME_H

#include <Arduino.h>

class Metronome {
  public:
    Metronome(unsigned long interval);
    bool intervalPassed();
    void reset();
    unsigned long getLastMillis();
    unsigned long getPassedTime();
  private:
    unsigned long m_interval;
    unsigned long m_lastMillis;
};

#endif
