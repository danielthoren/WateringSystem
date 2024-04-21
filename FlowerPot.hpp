
#ifndef FLOWER_POT
#define FLOWER_POT

#include "Sensor.hpp"

#include "CommonUtil.hpp"

#include <stdint.h>

class FlowerPot {
public:

  // Minimum amount of time between watering. If sensor triggers more
  // often than this, something is most likely wrong.
  static constexpr unsigned long m_minWaterInterval = msFromMin(30);

   // how long to wait between checking moisture level when idle
  static constexpr float m_idleWaittimeMs = 1000; //msFromMin(1);

  // how long to wait between updates when active
  static constexpr float m_activeWaittimeMs = 100;

  enum class State
  {
    // Reading moisture at a slow interval, waiting for it to reach the threshold.
    IDLE,
    // Performs the watering action, activating the pump for a fixed time.
    WATERING,
    // Waiting after watering is complete to let the water soak the soil.
    WAITING,
    // The moisture sensor triggered too often, likely due to a broken sensor.
    MIN_WATER_INTERVAL_ERROR
  };

  FlowerPot(ISensor const& sensor,
            uint8_t motorPin,
            uint8_t errorLedPin,
            uint8_t wateringTime_sek,
            uint8_t moistureThreshold = 40,
            uint8_t waitTimeAfterWatering_min = 10)
    : m_sensor{sensor},
      m_motorPin{motorPin},
      m_errorLedPin{errorLedPin},
      m_wateringTimeMs{msFromSec(wateringTime_sek)},
      m_moistureThreshold{moistureThreshold},
      m_waitTimeAfterWateringMs{msFromMin(waitTimeAfterWatering_min)}
  {
    pinMode(m_motorPin, OUTPUT);
    pinMode(m_errorLedPin, OUTPUT);
    digitalWrite(m_errorLedPin, LOW);
  }

  bool isIdle()
  {
    return
      m_state == State::IDLE ||
      m_state == State::WAITING ||
      m_state == State::MIN_WATER_INTERVAL_ERROR;
  }

  State update()
  {
    // Limit speed of updates
    if (isIdle() && millis() - m_lastUpdateTimeMs < m_idleWaittimeMs) {
      return m_state;
    } else if  (millis() - m_lastUpdateTimeMs < m_activeWaittimeMs) {
      return m_state;
    }
    m_lastUpdateTimeMs = millis();

    switch (m_state)
    {
      case State::IDLE:
        m_state = IdleState();
        break;
      case State::WATERING:
        m_state = WateringState();
        break;
      case State::WAITING:
        m_state = waitingState();
    }

    return m_state;
  }

private:

  State IdleState()
  {
    m_sensor.sample();
    if (m_sensor.isTriggered())
    {
      if (m_lastWaterTimeMs != 0 && millis() - m_lastWaterTimeMs < m_minWaterInterval)
      {
        Serial.println("Moisture sensor triggered earlier than the expected minimum time between watering, assuming failure...");
        digitalWrite(m_errorLedPin, HIGH);
        return State::MIN_WATER_INTERVAL_ERROR;
      }
      Serial.println("Start watering, transition to state 'WATERING'");

      // Start watering before going to watering state
      digitalWrite(m_motorPin, HIGH); // turn on the motor
      delay(50); // Wait for the relay to activate
      m_motorStartTimeMs = millis();

      return State::WATERING;
    }
    return State::IDLE;
  }

  State WateringState()
  {
    m_sensor.sample();
    if (m_sensor.isTriggered() == false ||
        (millis() - m_motorStartTimeMs > m_wateringTimeMs))
    {
      if (m_sensor.isTriggered() == false) {
        Serial.println("Moisture threshold reached, transition to state 'WAITING'");
      } else {
        Serial.println("Watering timeout reached, transition to state 'WAITING'");
      }

      digitalWrite(m_motorPin, LOW);  // turn off the motor
      m_lastWaterTimeMs = millis();
      return State::WAITING;
    }

    return State::WATERING;
  }

  State waitingState()
  {
    // Serial.print("time since watering: ");
    // Serial.print(millis() - m_lastWaterTimeMs);
    // Serial.print(" waitTime: ");
    // Serial.println(m_waitTimeAfterWateringMs);

    if (millis() - m_lastWaterTimeMs > m_waitTimeAfterWateringMs)
    {
      Serial.println("Waiting done, transition to state 'IDLE'");
      return State::IDLE;
    }
      return State::WAITING;
  }

  ISensor const& m_sensor;
  uint8_t const m_motorPin;
  uint8_t const m_errorLedPin;
  unsigned long const m_wateringTimeMs;
  uint8_t const m_moistureThreshold;
  uint32_t const m_waitTimeAfterWateringMs{};

  unsigned long m_lastWaterTimeMs{0};
  unsigned long m_motorStartTimeMs{0};
  unsigned long m_lastUpdateTimeMs{0};

  State m_state{State::IDLE};
};

#endif
