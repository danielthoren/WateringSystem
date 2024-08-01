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
  static constexpr float m_idleWaittimeMs = 1000;//msFromMin(10);

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

  FlowerPot() :
    m_pSensor{nullptr},
    m_motorPin{},
    m_errorLedPin{},
    m_wateringTimeMs{},
    m_waitTimeAfterWateringMs{}
  {}

  FlowerPot(ISensor* pSensor,
            uint8_t motorPin,
            uint8_t errorLedPin,
            uint8_t wateringTime_sek,
            uint8_t waitTimeAfterWatering_min = 10) :
    m_pSensor{pSensor},
    m_motorPin{motorPin},
    m_errorLedPin{errorLedPin},
    m_wateringTimeMs{msFromSec(wateringTime_sek)},
    m_waitTimeAfterWateringMs{msFromMin(waitTimeAfterWatering_min)}
  {
    ASSERT(pSensor != nullptr, "pSensor may not be null");
    ASSERT(wateringTime_sek != 0, "Watering time may not be 0");

    pinMode(m_motorPin, OUTPUT);
    digitalWrite(m_motorPin, LOW);

    pinMode(m_errorLedPin, OUTPUT);
    digitalWrite(m_errorLedPin, LOW);

    m_initialized = true;
  }

  bool isIdle()
  {
    return
      m_state == State::IDLE ||
      m_state == State::WAITING ||
      m_state == State::MIN_WATER_INTERVAL_ERROR;
  }

  bool isInitialized()
  {
    return m_initialized;
  }

  State update()
  {
    if (!isInitialized())
      return State::IDLE;

    // Limit speed of updates
    if (isIdle() && millis() - m_lastUpdateTimeMs < m_idleWaittimeMs)
    {
      return m_state;
    }
    else if  (millis() - m_lastUpdateTimeMs < m_activeWaittimeMs)
    {
      return m_state;
    }
    m_lastUpdateTimeMs = millis();

    m_pSensor->sample();
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
    if (m_pSensor->isTriggered())
    {
      if (m_lastWaterTimeMs != 0 && millis() - m_lastWaterTimeMs < m_minWaterInterval)
      {
        Serial.print(m_motorPin);
        Serial.println(F(": Moisture sensor triggered earlier than the expected minimum time between watering, assuming failure..."));

        digitalWrite(m_errorLedPin, HIGH);
        return State::MIN_WATER_INTERVAL_ERROR;
      }

      Serial.print(m_motorPin);

      Serial.print("Sensor value: { ");
      Serial.print(m_pSensor->getPercentageValue());
      Serial.print(" : ");
      Serial.print(m_pSensor->getRawValue());
      Serial.println("");

      Serial.println(F(": Start watering, transition to state 'WATERING'"));

      // Start watering before going to watering state
      digitalWrite(m_motorPin, HIGH); // turn on the motor
      m_motorStartTimeMs = millis();

      return State::WATERING;
    }
    return State::IDLE;
  }

  State WateringState()
  {
    // TODO: Add upper threshold for sensor
    if (false)//m_pSensor->isTriggered() == false)
    {
      Serial.print(m_motorPin);
      Serial.println(F(": Moisture threshold reached, transition to state 'WAITING'"));

      digitalWrite(m_motorPin, LOW);  // turn off the motor
      m_lastWaterTimeMs = millis();
      return State::WAITING;
    }
    else if (millis() - m_motorStartTimeMs > m_wateringTimeMs)
    {
      Serial.print(m_motorPin);
      Serial.println(F(": Watering timeout reached, transition to state 'WAITING'"));

      digitalWrite(m_motorPin, LOW);  // turn off the motor
      m_lastWaterTimeMs = millis();
      return State::WAITING;
    }

    return State::WATERING;
  }

  State waitingState()
  {
    if (millis() - m_lastWaterTimeMs > m_waitTimeAfterWateringMs)
    {
      Serial.print(m_motorPin);
      Serial.println(F(": Waiting done, transition to state 'IDLE'"));
      return State::IDLE;
    }
      return State::WAITING;
  }

  ISensor* m_pSensor;
  uint8_t m_motorPin;
  uint8_t m_errorLedPin;
  unsigned long m_wateringTimeMs;
  uint32_t m_waitTimeAfterWateringMs{};

  unsigned long m_lastWaterTimeMs{0};
  unsigned long m_motorStartTimeMs{0};
  unsigned long m_lastUpdateTimeMs{0};

  State m_state{State::IDLE};

  bool m_initialized{false};
};

#endif
