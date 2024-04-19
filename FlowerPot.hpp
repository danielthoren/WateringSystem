
#ifndef FLOWER_POT
#define FLOWER_POT

#include <stdint.h>

#include "Sensor.hpp"

static constexpr unsigned long MsFromSec(unsigned const sec) { return sec * 1000; }
static constexpr unsigned long MsFromMin(unsigned const min) { return min * 60 * 1000; }

class FlowerPot {
public:

  // Max amount of time to run the pump before aborting
  static constexpr unsigned long const m_maxPumpTime = MsFromSec(2);

  // Minimum amount of time between watering. If sensor triggers more
  // often than this, something is most likely wrong.
  static constexpr unsigned long m_minWaterInterval = MsFromMin(30);

  enum class State
  {
    IDLE,
    WATERING,
    EXTRA_WATERING_TIME,
    MIN_WATER_INTERVAL_ERROR
  };

  FlowerPot(ISensor const& sensor,
            uint8_t motorPin,
            uint8_t errorLedPin,
            unsigned moistureThreshold = 40,
            unsigned extraWateringTimeSek = 1)
    : m_sensor{sensor},
      m_motorPin{motorPin},
      m_errorLedPin{errorLedPin},
      m_moistureThreshold{moistureThreshold},
      m_extraWateringTimeSek{extraWateringTimeSek}
  {
    pinMode(m_motorPin, OUTPUT);
    pinMode(m_errorLedPin, OUTPUT);
    digitalWrite(m_errorLedPin, LOW);
  }

  bool isIdle()
  {
    return m_state == State::IDLE ||
      m_state == State::MIN_WATER_INTERVAL_ERROR;
  }

  State update()
  {
    switch (m_state)
    {
      case State::IDLE:
        m_state = IdleState();
        break;
      case State::WATERING:
        m_state = WateringState();
        break;
      case State::EXTRA_WATERING_TIME:
        m_state = ExtraWateringTimeState();

      default:
        m_sensor.sample();
        m_sensor.isTriggered();
    }

    return m_state;
  }

private:

  State IdleState()
  {
    m_sensor.sample();
    if (m_sensor.isTriggered())
    {
      if (m_lastWaterTime != 0 && millis() - m_lastWaterTime < m_minWaterInterval)
      {
        Serial.println("Moisture sensor triggered earlier than the expected minimum time between watering, assuming failure...");
        digitalWrite(m_errorLedPin, HIGH);
        return State::MIN_WATER_INTERVAL_ERROR;
      }
      Serial.println("Start watering, transition to state 'WATERING'");

      // Start watering before going to watering state
      digitalWrite(m_motorPin, HIGH); // turn on the motor
      delay(50); // Wait for the relay to activate
      m_motorStartTime = millis();

      return State::WATERING;
    }
    return State::IDLE;
  }

  State WateringState()
  {
    m_sensor.sample();
    if (m_sensor.isTriggered() == false)
    {
      Serial.println("Moisture threshold reached, transition to state 'EXTRA_WATERING_TIME'");
      m_lastWaterTime = millis();
      return State::EXTRA_WATERING_TIME;
    }
    else if (millis() - m_motorStartTime > m_maxPumpTime)
    {
      Serial.println("Watering timeout reached transition to state 'EXTRA_WATERING_TIME'");
      digitalWrite(m_motorPin, LOW);  // turn off the motor
      m_lastWaterTime = millis();
      return State::IDLE;
    }

    return State::WATERING;
  }

  State ExtraWateringTimeState()
  {
    if (millis() - m_lastWaterTime > MsFromSec(m_extraWateringTimeSek))
    {
      Serial.println("Stop watering, transition to state 'IDLE'");
      digitalWrite(m_motorPin, LOW);  // turn off the motor
      m_lastWaterTime = millis();
      return State::IDLE;
    }
    return State::EXTRA_WATERING_TIME;
  }

  ISensor const& m_sensor;
  uint8_t const m_motorPin;
  uint8_t const m_errorLedPin;
  unsigned const m_extraWateringTimeSek;
  unsigned const m_moistureThreshold;

  unsigned long m_lastWaterTime{0};
  State m_state{State::IDLE};
  unsigned long m_motorStartTime{0};
};

#endif
