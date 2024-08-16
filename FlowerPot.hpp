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
  static constexpr float m_idleWaittimeMs = 1000; //msFromMin(10);

  // how long to wait between updates when active
  static constexpr float m_activeWaittimeMs = 100;

  // Soft pwm dutycycle length
  static constexpr unsigned m_dutyCycleLenUs = 1000;

  enum class State
  {
    // Initial state, changed to IDLE once initialized
    UNINITIALIZED,
    // Reading moisture at a slow interval, waiting for it to reach the threshold.
    // Once the threshold is reached, the pot is watered in a blocking way.
    IDLE,
    // Waiting after watering is complete to let the water soak the soil.
    WAITING,
    // The moisture sensor triggered too often, likely due to a broken sensor.
    MIN_WATER_INTERVAL_ERROR
  };

  FlowerPot() :
    m_pSensor{nullptr},
    m_motorPin{},
    m_motorSpeed{},
    m_wateringTimeMs{},
    m_waitTimeAfterWateringMs{}
  {}

  FlowerPot(ISensor* pSensor,
            uint8_t motorPin,
            uint8_t motorSpeed,
            uint8_t wateringTime_sek,
            uint8_t waitTimeAfterWatering_min = 10) :
    m_pSensor{pSensor},
    m_motorPin{motorPin},
    m_motorSpeed{motorSpeed},
    m_wateringTimeMs{msFromSec(wateringTime_sek)},
    m_waitTimeAfterWateringMs{msFromMin(waitTimeAfterWatering_min)}
  {
    ASSERT(pSensor != nullptr, "pSensor may not be null");
    ASSERT(wateringTime_sek != 0, "Watering time may not be 0");

    pinMode(m_motorPin, OUTPUT);
    digitalWrite(m_motorPin, LOW);

    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);

    m_state = State::IDLE;
  }

  bool isInitialized() const
  {
    return m_state != State::UNINITIALIZED;
  }

  State update()
  {
    if (!isInitialized())
      return State::UNINITIALIZED;

    // Limit speed of updates
    if (millis() - m_lastUpdateTimeMs < m_idleWaittimeMs)
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
      case State::WAITING:
        m_state = waitingState();
      default:
        break;
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

        return State::MIN_WATER_INTERVAL_ERROR;
      }

      Serial.println(F(": Start watering"));

      Serial.print(m_motorPin);
      Serial.print("Sensor value: { ");
      Serial.print(m_pSensor->getPercentageValue());
      Serial.print(" : ");
      Serial.print(m_pSensor->getRawValue());
      Serial.println("}");

      uint16_t on_time = (static_cast<double>(m_motorSpeed) / 100) * m_dutyCycleLenUs;
      uint16_t off_time = m_dutyCycleLenUs - on_time;

      // Run the pump using software PWM for the configured duration
      unsigned long startTime = millis();
      while (millis() - startTime < m_wateringTimeMs)
      {
        digitalWrite(m_motorPin, HIGH);
        delayMicroseconds(on_time);
        digitalWrite(m_motorPin, LOW);
        delayMicroseconds(off_time);
      }
      m_lastWaterTimeMs = millis();

      return State::WAITING;
    }
    return State::IDLE;
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
  uint8_t m_motorSpeed;
  unsigned long m_wateringTimeMs;
  uint32_t m_waitTimeAfterWateringMs{};

  unsigned long m_lastWaterTimeMs{0};
  unsigned long m_lastUpdateTimeMs{0};

  State m_state{State::UNINITIALIZED};
};

#endif
