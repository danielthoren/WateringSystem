
#ifndef SENSOR
#define SENSOR

#include "Filter.hpp"
#include "CommonUtil.hpp"

class ISensor
{
public:
  ISensor() = default;
  virtual ~ISensor() = default;

  virtual bool isTriggered() const = 0;
  virtual void sample() = 0;

  virtual unsigned getPercentageValue() const = 0;
  virtual unsigned getRawValue() const = 0;

  bool isInitialized() const { return m_initialized; }

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
    m_lowerTriggerThresholdPercent{},
    m_invertTrigger{},
    m_filter{}
  {}

  ResistiveMoistureSensor(uint8_t dataPin,
                          uint8_t powerPin,
                          unsigned minSensorRange,
                          unsigned maxSensorRange,
                          uint8_t lowerTriggerThresholdPercent,
                          bool invertTrigger = false,
                          double filterAlpha = 0.8) :
    m_dataPin{dataPin},
    m_powerPin{powerPin},
    m_minSensorRange{minSensorRange},
    m_maxSensorRange{maxSensorRange},
    m_lowerTriggerThresholdPercent{lowerTriggerThresholdPercent},
    m_invertTrigger{invertTrigger},
    m_filter{filterAlpha, static_cast<double>(maxSensorRange)}
  {
    ASSERT(minSensorRange != maxSensorRange, "The lower and upper sensor range must not be equal");
    ASSERT(lowerTriggerThresholdPercent < 100, "The trigger threshold cannot be larger than, or equal to, 100%");

    pinMode(m_powerPin, OUTPUT);

    // If sensor disconnected, pullup ensures that it registers full
    // moisture to avoid motor activating

    // TODO: Capacitive moisture sensor does not work with pullup
#if defined KITCHEN
    digitalWrite(m_powerPin, LOW);
    pinMode(m_dataPin, INPUT);
#elif defined LIVING_ROOM_BIG_WINDOW
    digitalWrite(m_powerPin, HIGH);
    pinMode(m_dataPin, INPUT);
#endif

    m_initialized = true;
  }

  void sample() override
  {
    ASSERT(isInitialized(), "Sensor not initialized!");

#if defined KITCHEN
    // Power on sensor
    digitalWrite(m_powerPin, HIGH);
    // Allow power to settle
    delay(100);
    m_filter.filter(analogRead(m_dataPin));

    // Power off sensor
    digitalWrite(m_powerPin, LOW);

#elif defined LIVING_ROOM_BIG_WINDOW
    m_filter.filter(analogRead(m_dataPin));
#endif
  }

  bool isTriggered() const override
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    unsigned sensorValue = getPercentageValue();

    if (m_invertTrigger)
      return sensorValue >= m_lowerTriggerThresholdPercent;
    else
      return sensorValue < m_lowerTriggerThresholdPercent;
  }

  unsigned getPercentageValue() const override
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    unsigned boundedValue = constrain(m_filter.getValue(),
                                      min(m_minSensorRange, m_maxSensorRange),
                                      max(m_minSensorRange, m_maxSensorRange));

    return map(boundedValue, m_minSensorRange, m_maxSensorRange, 0, 100);
  }

  unsigned getRawValue() const override
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    return m_filter.getValue();
  }

  void setTriggerThreshold(uint8_t threshold)
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    m_lowerTriggerThresholdPercent = threshold;
  }

private:

  uint8_t m_dataPin;
  uint8_t m_powerPin;
  unsigned m_minSensorRange;
  unsigned m_maxSensorRange;
  uint8_t m_lowerTriggerThresholdPercent;
  bool m_invertTrigger;
  LowPassFilter<unsigned> m_filter;
};

#endif
