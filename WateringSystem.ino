
#include "FlowerPot.hpp"
#include "Sensor.hpp"

constexpr float moistureRawMax = 1023.00;
constexpr float moistureRawMin = 0.0;
constexpr uint8_t errorPin = 12;
constexpr uint8_t wateringTime = 2;
constexpr unsigned moistureThreshold = 40;

constexpr float idleWaittimeMs = 1000; //MsFromMin(1); // how long to wait between checking moisture level when idle
constexpr float activeWaittimeMs = 100; // how long to wait between updates when active

constexpr uint8_t sensor1DataPin = A0;
constexpr uint8_t sensor1PowerPin = 11;
constexpr uint8_t motor1Pin = 3;

ResistiveMoistureSensor sensor1{
    sensor1DataPin,
    sensor1PowerPin,
    moistureRawMin,
    moistureRawMax,
    moistureThreshold};

FlowerPot pot1{sensor1, motor1Pin, errorPin, wateringTime, moistureThreshold};

constexpr uint8_t sensor2DataPin = A1;
constexpr uint8_t sensor2PowerPin = 10;
constexpr uint8_t motor2Pin = 4;

ResistiveMoistureSensor sensor2{
    sensor2DataPin,
    sensor2PowerPin,
    moistureRawMin,
    moistureRawMax,
    moistureThreshold};

FlowerPot pot2{sensor2, motor2Pin, errorPin, wateringTime, moistureThreshold};

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  pot1.update();
  pot2.update();
}

void Calibration(int analolgReadPin) {
  int moistureVal = analogRead(analolgReadPin);  // read the input on analog pin 0:
  Serial.println(moistureVal); // print out the analog val
  delay(100);
}
