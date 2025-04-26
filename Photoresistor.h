#ifndef MY_PHOTORESISTOR_H
#define MY_PHOTORESISTOR_H

#include <Arduino.h>

class Photoresistor {
  private:
    int m_pin;
      int m_threshold;
      int m_environmentalBrightness;
      float m_brightnessDrop;
  
      unsigned long m_lastCoverTime = 0;
      const unsigned long m_debounceDelay = 1000;
  
      bool m_covered = false;
      unsigned long m_coverStart = 0;
      unsigned long m_coverEnd = 0;
      unsigned long m_lastDuration = 0;
  public:
    Photoresistor(int analogPin, byte dropPercent);

    void setEnvironmentalBrightness(int brightness, bool verbose = false);
    int getEnvironmentalBrightness();
    void updateEnvironmentalBrightness(unsigned long measurementTime);

    int getThreshold();
    void updateThreshold(bool verbose = false);
    
    int read();
    bool isDark();
    bool isCoveredInLessThanTimePeriod(unsigned long timePeriod = 1000, bool verbose = false);
};

#endif
