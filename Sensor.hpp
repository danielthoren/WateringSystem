
#ifndef SENSOR
#define SENSOR

#include "Filter.hpp"

class ISensor
{
public:
  ISensor() = default;
  virtual ~ISensor() = default;

  virtual bool isTriggered() = 0;
  virtual void sample() = 0;
};

class ResistiveMoistureSensor : public ISensor
{
public:
  ResistiveMoistureSensor(uint8_t dataPin,
                          uint8_t powerPin,
                          unsigned minSensorRange,
                          unsigned maxSensorRange,
                          uint8_t triggerThresholdPercent,
                          bool invertTrigger = false,
                          double filterAlpha = 0.3) :
    m_dataPin{dataPin},
    m_powerPin{powerPin},
    m_minSensorRange{minSensorRange},
    m_maxSensorRange{maxSensorRange},
    m_triggerThresholdPercent{triggerThresholdPercent},
    m_invertTrigger{invertTrigger},
    m_filter{filterAlpha}
  {
    pinMode(m_powerPin, OUTPUT);
    digitalWrite(m_powerPin, LOW);
  }

  void sample() override
  {
    // Power on sensor
    digitalWrite(m_powerPin, HIGH);
    // Allow power to settle
    delay(10);
    m_filter.filter(analogRead(m_dataPin));

    // Power off sensor
    digitalWrite(m_powerPin, LOW);
  }

  bool isTriggered() override
  {
    unsigned sensorValue = getPercentageValue();
    Serial.println(sensorValue);
    if (m_invertTrigger)
      return sensorValue >= m_triggerThresholdPercent;
    else
      return sensorValue < m_triggerThresholdPercent;
  }

private:

  unsigned getPercentageValue()
  {
    float sensorRange = m_maxSensorRange - m_minSensorRange;
    return 100 - ((static_cast<float>(m_filter.getValue()) / sensorRange) * 100);
  }

  uint8_t const m_dataPin;
  uint8_t const m_powerPin;
  unsigned const m_minSensorRange;
  unsigned const m_maxSensorRange;
  uint8_t const m_triggerThresholdPercent;
  bool const m_invertTrigger;
  LowPassFilter<unsigned> const m_filter;
};

#endif
