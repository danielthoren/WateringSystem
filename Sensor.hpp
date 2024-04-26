
#ifndef SENSOR
#define SENSOR

#include "Filter.hpp"

#include "CommonUtil.hpp"

class ISensor
{
public:
  ISensor() = default;
  virtual ~ISensor() = default;

  virtual bool isTriggered() = 0;
  virtual void sample() = 0;

  bool isInitialized() { return m_initialized; }

protected:
  bool m_initialized{false};
};

class ResistiveMoistureSensor : public ISensor
{
public:
  ResistiveMoistureSensor() :
    m_dataPin{},
    m_powerPin{},
    m_minSensorRange{},
    m_maxSensorRange{},
    m_triggerThresholdPercent{},
    m_invertTrigger{},
    m_filter{}
  {}

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
    ASSERT(minSensorRange != maxSensorRange, "The lower and upper sensor range must not be equal");
    ASSERT(triggerThresholdPercent < 100, "The trigger threshold cannot be larger than, or equal to, 100%");

    pinMode(m_powerPin, OUTPUT);
    digitalWrite(m_powerPin, LOW);

    // If sensor disconnected, pullup ensures that it registers full
    // moisture to avoid motor activating
    pinMode(m_dataPin, INPUT_PULLUP);

    m_initialized = true;
  }

  void sample() override
  {
    if (!isInitialized())
      return;

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
    if (!isInitialized())
      return false;

    unsigned sensorValue = getPercentageValue();

    Serial.print("Sensor ");
    Serial.print(m_dataPin);
    Serial.print(": ");
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

  uint8_t m_dataPin;
  uint8_t m_powerPin;
  unsigned m_minSensorRange;
  unsigned m_maxSensorRange;
  uint8_t m_triggerThresholdPercent;
  bool m_invertTrigger;
  LowPassFilter<unsigned> m_filter;
};

#endif
