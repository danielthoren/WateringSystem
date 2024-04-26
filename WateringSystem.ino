
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

ResistiveMoistureSensor sensors[numPots];

// Flower pots
constexpr uint8_t wateringTime = 2;

FlowerPot pots[numPots];

void setup()
{
  ASSERT(numPots <= hwSupportedPotNum, "HW supported pot number exceeded");
  Serial.begin(9600);

  for (size_t i{0}; i < numPots; ++i)
  {
    sensors[i] = {
      .dataPin = 5 - i, // Sensor 1 on pin 5, sensor 2 on pin 4, etc
      .powerPin = sensorPowerPin,
      .minSensorRange = moistureRawMin,
      .maxSensorRange = moistureRawMax,
      .triggerThresholdPercent = moistureThreshold
    };

    pots[i] = {
      &sensors[i],
      7 + i, // Motor 1 on pin 7, motor 2 on pin 8, etc
      errorPin,
      wateringTime
    };
  }
}

void loop()
{
  for (size_t i{0}; i < numPots; ++i)
  {
    pots[i].update();
  }
}
