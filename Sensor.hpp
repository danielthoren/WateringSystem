
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

  virtual unsigned getPercentageValue() = 0;
  virtual unsigned getRawValue() = 0;

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
                          double filterAlpha = 0.8) :
    m_dataPin{dataPin},
    m_powerPin{powerPin},
    m_minSensorRange{minSensorRange},
    m_maxSensorRange{maxSensorRange},
    m_triggerThresholdPercent{triggerThresholdPercent},
    m_invertTrigger{invertTrigger},
    m_filter{filterAlpha, maxSensorRange}
  {
    ASSERT(minSensorRange != maxSensorRange, "The lower and upper sensor range must not be equal");
    ASSERT(triggerThresholdPercent < 100, "The trigger threshold cannot be larger than, or equal to, 100%");

    pinMode(m_powerPin, OUTPUT);
    digitalWrite(m_powerPin, LOW);

    // If sensor disconnected, pullup ensures that it registers full
    // moisture to avoid motor activating

    // TODO: Capacitive moisture sensor does not work with pullup
#ifdef KITCHEN
    pinMode(m_dataPin, INPUT_PULLUP);
#else
    pinMode(m_dataPin, INPUT);
#endif

    m_initialized = true;
  }

  void sample() override
  {
    if (!isInitialized())
      return;

    // Power on sensor
    digitalWrite(m_powerPin, HIGH);
    // Allow power to settle
    delay(100);
    m_filter.filter(analogRead(m_dataPin));

    // Power off sensor
    digitalWrite(m_powerPin, LOW);
  }

  bool isTriggered() override
  {
    if (!isInitialized())
      return false;

    unsigned sensorValue = getPercentageValue();

    if (m_invertTrigger)
      return sensorValue >= m_triggerThresholdPercent;
    else
      return sensorValue < m_triggerThresholdPercent;
  }

  unsigned getPercentageValue() override
  {
    // TODO: Bound the raw value to the max/min range to avoid strange percentage values
    return map(m_filter.getValue(), m_minSensorRange, m_maxSensorRange, 0, 100);
  }

  unsigned getRawValue() override
  {
    return m_filter.getValue();
  }

private:

  uint8_t m_dataPin;
  uint8_t m_powerPin;
  unsigned m_minSensorRange;
  unsigned m_maxSensorRange;
  uint8_t m_triggerThresholdPercent;
  bool m_invertTrigger;
  LowPassFilter<unsigned> m_filter;
};

#endif
