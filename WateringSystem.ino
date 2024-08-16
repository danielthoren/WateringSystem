
/* #define KITCHEN */
#define LIVING_ROOM_BIG_WINDOW

#include "FlowerPot.hpp"
#include "Sensor.hpp"

constexpr size_t hwSupportedPotNum = 6;

#if defined KITCHEN

constexpr size_t numPots = 4;

constexpr unsigned moistureRawMax = 400;
constexpr unsigned moistureRawMin = 990;

#elif defined LIVING_ROOM_BIG_WINDOW

constexpr size_t numPots = 6;

constexpr unsigned moistureRawMax = 320;
constexpr unsigned moistureRawMin = 687;

#endif

// Sensors
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
  double alpha = 1;

  for (size_t i{0}; i < numPots; ++i)
  {
    sensors[i] = {
      .dataPin = sensorPins[i],
      .powerPin = sensorPowerPin,
      .minSensorRange = moistureRawMin,
      .maxSensorRange = moistureRawMax,
      .triggerThresholdPercent = moistureThreshold,
      .invertTrigger = false,
      .filterAplha = alpha
    };
  }

  // Flowers left to right
#if defined KITCHEN

  alpha = 0.8;

  pots[0] = {
    &sensors[0],
    motorPins[0],
    50,
    5
    };

  pots[1] = {
    &sensors[1],
    motorPins[1],
    60,
    4
    };

  pots[2] = {
    &sensors[2],
    motorPins[2],
    60,
    4
    };

  pots[3] = {
    &sensors[3],
    motorPins[3],
    60,
    4
    };

#elif defined LIVING_ROOM_BIG_WINDOW

  alpha = 1;

  //Left-most plant
  /* pots[0] = { */
  /*   &sensors[0], */
  /*   motorPins[0], */
  /*   100, */
  /*   6 */
  /*   }; */

  pots[1] = {
    &sensors[1],
    motorPins[1],
    50,
    4
    };

  pots[2] = {
    &sensors[2],
    motorPins[2],
    50,
    4
    };

    pots[3] = {
    &sensors[3],
    motorPins[3],
    55,
    2
    };

    pots[4] = {
    &sensors[4],
    motorPins[4],
    100,
    4
    };

    pots[5] = {
    &sensors[5],
    motorPins[5],
    45,
    5
    };

#endif
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
        Serial.print(" { ");
        Serial.print(sensors[i].getPercentageValue());
        Serial.print(" : ");
        Serial.print(sensors[i].getRawValue());
        Serial.print(" } ");
      }
      Serial.println("");
    }
  }
}
