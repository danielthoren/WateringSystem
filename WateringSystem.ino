#include "FlowerPot.hpp"
#include "Sensor.hpp"

constexpr size_t hwSupportedPotNum = 6;

constexpr uint8_t errorPin = 12;
constexpr size_t numPots = 6;

// Sensors
constexpr float moistureRawMax = 1023.00;
constexpr float moistureRawMin = 0.0;
constexpr uint8_t sensorPowerPin = 13;
constexpr unsigned moistureThreshold = 40;

constexpr uint8_t sensorPins[] = {A0, A1, A2, A3, A6, A7};
ResistiveMoistureSensor sensors[numPots];

// Flower pots
constexpr uint8_t motorPins[] = {12, 11, 10, 9, 8, 7};
FlowerPot pots[numPots];

void setup()
{
  ASSERT(numPots <= hwSupportedPotNum, "HW supported pot number exceeded");
  Serial.begin(9600);

  for (size_t i{0}; i < numPots; ++i)
  {
    sensors[i] = {
      .dataPin = sensorPins[i],
      .powerPin = sensorPowerPin,
      .minSensorRange = moistureRawMin,
      .maxSensorRange = moistureRawMax,
      .triggerThresholdPercent = moistureThreshold
    };
  }

  pots[0] = {
    &sensors[0],
    motorPins[0],
    errorPin,
    2
    };

  pots[1] = {
    &sensors[1],
    motorPins[1],
    errorPin,
    2
    };

  pots[2] = {
    &sensors[2],
    motorPins[2],
    errorPin,
    2
    };

  pots[3] = {
    &sensors[3],
    motorPins[3],
    errorPin,
    3
    };
}

constexpr unsigned long sensorPrintTimeout = FlowerPot::m_idleWaittimeMs;
unsigned long lastSensorPrintTime = millis();

void loop()
{
  for (size_t i{0}; i < numPots; ++i)
  {
    pots[i].update();

    if (millis() - lastSensorPrintTime > sensorPrintTimeout)
    {
      lastSensorPrintTime = millis();

      for (size_t i{0}; i < numPots; ++i)
      {
        Serial.print(sensors[i].getPercentageValue());
        Serial.print("  ");
      }
      Serial.println("");
    }
  }
}
