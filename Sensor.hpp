
#ifndef SENSOR
#define SENSOR

#include "Filter.hpp"
#include "CommonUtil.hpp"

#include "MenuLib/MenuItemList.hpp"
#include "MenuLib/MenuItemEditField.hpp"
#include "MenuLib/MenuItemText.hpp"
#include "MenuLib/String.hpp"

using MenuLib::MenuItemList;
using MenuLib::MenuItemEditField;
using MenuLib::MenuItemText;
using MenuLib::IString;
using MenuLib::ProgMemString;

class Sensor : public MenuItemList
{
public:
  Sensor() :
    MenuItemList{&m_menuLabelStr, m_topMenuItemsArr},
    m_dataPin{},
    m_powerPin{},
    m_minSensorRange{},
    m_maxSensorRange{},
    m_lowerTriggerThresholdPercent{},
    m_invertTrigger{},
    m_filter{}
  {}

  Sensor(uint8_t dataPin,
         uint8_t powerPin,
         unsigned minSensorRange,
         unsigned maxSensorRange,
         uint8_t lowerTriggerThresholdPercent,
         bool invertTrigger = false,
         double filterAlpha = 0.8) :
    MenuItemList{&m_menuLabelStr, m_topMenuItemsArr},
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
#else
    digitalWrite(m_powerPin, HIGH);
    pinMode(m_dataPin, INPUT);
#endif

    m_initializedSensor = true;
  }

  void sample()
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

#else
    m_filter.filter(analogRead(m_dataPin));
#endif
  }

  bool isTriggered() const
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    unsigned sensorValue = getPercentageValue();

    if (m_invertTrigger)
      return sensorValue >= m_lowerTriggerThresholdPercent;
    else
      return sensorValue < m_lowerTriggerThresholdPercent;
  }

  unsigned getPercentageValue() const
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    unsigned boundedValue = constrain(m_filter.getValue(),
                                      min(m_minSensorRange, m_maxSensorRange),
                                      max(m_minSensorRange, m_maxSensorRange));

    return map(boundedValue, m_minSensorRange, m_maxSensorRange, 0, 100);
  }

  unsigned getRawValue() const
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    return m_filter.getValue();
  }

  virtual uint8_t getTriggerThreshold() const
  {
    return m_lowerTriggerThresholdPercent;
  }

  void setTriggerThreshold(uint8_t threshold)
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    ASSERT(0 < threshold && threshold < 100, "Threshold must be in the interval 0 < threshold < 100");

    m_lowerTriggerThresholdPercent = threshold;
  }

  virtual unsigned getMinValue() const
  {
    return m_minSensorRange;
  }

  virtual unsigned getMaxValue() const
  {
    return m_maxSensorRange;
  }

  void setMinMaxValues(unsigned minValue, unsigned maxValue)
  {
    ASSERT(isInitialized(), "Sensor not initialized!");
    ASSERT(minValue < m_analogReadMaxValue && maxValue < m_analogReadMaxValue,
           "Values must be less than max analogRead value!");
    ASSERT(minValue < maxValue, "minValue must be less than maxValue!");

    m_minSensorRange = minValue;
    m_maxSensorRange = maxValue;
  }

private:

  uint8_t m_dataPin;
  uint8_t m_powerPin;
  unsigned m_minSensorRange;
  unsigned m_maxSensorRange;
  uint8_t m_lowerTriggerThresholdPercent;
  bool m_invertTrigger;
  LowPassFilter<unsigned> m_filter;
  bool m_initializedSensor{false};

/*******************************************************************************
 ***                            GUI logic and storage                         ***
 *******************************************************************************/

public:

private:

  /************************ Trigger threshold ************************/

  static constexpr uint8_t m_threshStep = 10;
  static constexpr uint8_t m_threshMinVal = 10;
  static constexpr uint8_t m_threshMaxVal = 90;

  static char const m_trigThreshFormat[LCD_COLS] PROGMEM;
  ProgMemString m_trigThreshFormatStr{m_trigThreshFormat, sizeof(m_trigThreshFormat)};

  MenuItemEditField m_trigThreshMenuItem{
    &m_trigThreshFormatStr,
    &m_lowerTriggerThresholdPercent,
    m_threshStep, m_threshMinVal, m_threshMaxVal
  };

  /************************ Max/Min values ************************/

  static char const m_sensorMaxValLabel[LCD_COLS] PROGMEM;
  ProgMemString m_sensorMaxValLabelStr{m_sensorMaxValLabel, sizeof(m_sensorMaxValLabel)};
  MenuItemText m_sensorMaxVal{&m_sensorMaxValLabelStr};

  static char const m_sensorMinValLabel[LCD_COLS] PROGMEM;
  ProgMemString m_sensorMinValLabelStr{m_sensorMinValLabel, sizeof(m_sensorMinValLabel)};
  MenuItemText m_sensorMinVal{&m_sensorMinValLabelStr};

  /************************ Top menu ************************/

  MenuItemBase* m_topMenuItems[3] = {
    dynamic_cast<MenuItemBase*>(&m_trigThreshMenuItem),
    dynamic_cast<MenuItemBase*>(&m_sensorMaxVal),
    dynamic_cast<MenuItemBase*>(&m_sensorMinVal),
  };
  Array<MenuItemBase*> m_topMenuItemsArr{m_topMenuItems, 3};

  static char const m_menuLabel[LCD_COLS] PROGMEM;
  ProgMemString m_menuLabelStr{m_menuLabel, sizeof(m_menuLabel)};
};

char const Sensor::m_trigThreshFormat[LCD_COLS] PROGMEM = "Trig level: %d%%";

char const Sensor::m_sensorMaxValLabel[LCD_COLS] PROGMEM = "Max val: %d";
char const Sensor::m_sensorMinValLabel[LCD_COLS] PROGMEM = "Min val: %d";

char const Sensor::m_menuLabel[LCD_COLS] PROGMEM = "Sensor";

#endif
