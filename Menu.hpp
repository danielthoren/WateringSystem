#ifndef MENU
#define MENU

#include "MenuLib/MenuItemBase.hpp"
#include "MenuLib/MenuItemList.hpp"
#include "MenuLib/MenuItemText.hpp"
#include "MenuLib/MenuItemEditField.hpp"
#include "MenuLib/MenuItemCallback.hpp"
#include "MenuLib/Menu.hpp"
#include "MenuLib/common.hpp"
#include "MenuLib/String.hpp"

#include "CommonUtil.hpp"

#include "MoistureLevelScreen.hpp"

using namespace MenuLib;

class SensorMenu : public MenuItemList
{
public:
  SensorMenu() :
    MenuItemList{&m_sensorMenuLabelStr, m_sensorItemsArr}
  {}

  void setSensor(Sensor* pSensor)
  {
    ASSERT(pSensor != nullptr, "pSensor may not be null");
    m_pSensor = pSensor;
    m_trigLevelItem = MenuItemEditField<uint8_t>{&m_trigLevelformatStr, &m_pSensor->m_triggerThresh, trigStep, trigMinVal, trigMaxVal};
    m_maxValSetManual =
      MenuItemEditField<unsigned>{&m_maxMinSetManualStr, &m_pSensor->m_maxSensorRange, maxMinStep, 0, Sensor::m_analogReadMaxValue};
  }

private:
  Sensor* m_pSensor{nullptr};

  /************************ Trigger level ************************/
  static char const m_trigLevelformat[LCD_COLS] PROGMEM;
  ProgMemString m_trigLevelformatStr{m_trigLevelformat, sizeof(m_trigLevelformat)};

  static constexpr uint8_t trigStep = 10;
  static constexpr uint8_t trigMinVal = 10;
  static constexpr uint8_t trigMaxVal = 90;

  MenuItemEditField<uint8_t> m_trigLevelItem{&m_trigLevelformatStr, nullptr, trigStep, trigMinVal, trigMaxVal};

  /************************ Max value ************************/

  static char const m_maxMinSetManual[LCD_COLS] PROGMEM;
  ProgMemString m_maxMinSetManualStr{m_maxMinSetManual, sizeof(m_maxMinSetManual)};
  static constexpr uint8_t maxMinStep = 50;

  static char const m_maxMinSetCurr[LCD_COLS] PROGMEM;
  ProgMemString m_maxMinSetCurrStr{m_maxMinSetCurr, sizeof(m_maxMinSetCurr)};

  MenuItemEditField<unsigned> m_maxValSetManual{&m_maxMinSetManualStr, nullptr, maxMinStep, 0, Sensor::m_analogReadMaxValue};

  MenuItemBase* m_maxItems[1] = {
    dynamic_cast<MenuItemBase*>(&m_maxValSetManual)
  };
  Array<MenuItemBase*> m_maxItemsArr{m_maxItems, sizeof(m_maxItems) / sizeof(MenuItemBase*)};

  static char const m_sensorMaxValLabel[LCD_COLS] PROGMEM;
  ProgMemString m_sensorMaxValLabelStr{m_sensorMaxValLabel, sizeof(m_sensorMaxValLabel)};
  MenuItemList m_sensorMaxVal{&m_sensorMaxValLabelStr, m_maxItemsArr};

  /************************ Min value ************************/
  static char const m_sensorMinValLabel[LCD_COLS] PROGMEM;
  ProgMemString m_sensorMinValLabelStr{m_sensorMinValLabel, sizeof(m_sensorMinValLabel)};
  MenuItemText m_sensorMinVal{&m_sensorMinValLabelStr};

  /************************ Top Menu ************************/

  MenuItemBase* m_sensorItems[3] = {
    dynamic_cast<MenuItemBase*>(&m_trigLevelItem),
    dynamic_cast<MenuItemBase*>(&m_sensorMaxVal),
    dynamic_cast<MenuItemBase*>(&m_sensorMinVal)
  };
  Array<MenuItemBase*> m_sensorItemsArr{m_sensorItems, sizeof(m_sensorItems) / sizeof(MenuItemBase*)};

  static char const m_sensorMenuLabel[LCD_COLS] PROGMEM;
  ProgMemString m_sensorMenuLabelStr{m_sensorMenuLabel, sizeof(m_sensorMenuLabel)};
};

char const SensorMenu::m_trigLevelformat[LCD_COLS] PROGMEM = "Trig level: %d%%";


char const SensorMenu::m_maxMinSetManual[LCD_COLS] PROGMEM = "Set manual:%d";
char const SensorMenu::m_maxMinSetCurr[LCD_COLS] PROGMEM   = "Set curr:  %d";
char const SensorMenu::m_sensorMaxValLabel[LCD_COLS] PROGMEM = "Max val: %d";
char const SensorMenu::m_sensorMinValLabel[LCD_COLS] PROGMEM = "Min val: %d";

char const SensorMenu::m_sensorMenuLabel[LCD_COLS] PROGMEM = "Sensor";



class PotMenu : public MenuItemList
{
public:
  PotMenu()
    : MenuItemList{&m_potMenuItemLabelStr, m_topItemsArr}
  {}

  PotMenu(FlowerPot* pot, uint8_t potNum)
    : MenuItemList{&m_potMenuItemLabelStr, m_topItemsArr},
      m_pot{pot},
      m_potNum{potNum},
      m_initialized{true},
      m_sensorMenu{}
  {
    ASSERT(m_pot != nullptr, "m_pot may not be null!");

    snprintf(static_cast<char*>(m_potMenuItemLabel), LCD_COLS, "Pot %u", potNum);

    m_sensorMenu.setSensor(m_pot->getSensor());
  }

  void setPot(FlowerPot* pPot)
  {
    ASSERT(pPot != nullptr, "pPot may not be null");
    m_pot = pPot;
    m_sensorMenu.setSensor(pPot->getSensor());
  }

private:
  FlowerPot* m_pot;
  uint8_t m_potNum;
  bool m_initialized{false};

  /************************ Sensor menu ************************/

  SensorMenu m_sensorMenu;

  /************************ Motor menu ************************/

  char m_motorRunTimeLabel[LCD_COLS] = "Run time: %d";
  RamString m_motorRunTimeLabelStr{m_motorRunTimeLabel, LCD_COLS};
  MenuItemText m_motorRunTime{&m_motorRunTimeLabelStr};

  // MenuItemBase* m_motorItems[1] = {
  //   dynamic_cast<MenuItemBase*>(&m_motorRunTime)
  // };

  // char m_motorMenuLabel[LCD_COLS] = "Motor";
  // MenuItemList m_motorMenu{m_motorMenuLabel, Array<MenuItemBase*>{m_motorItems, 1}};

  /************************ Top menu ************************/

  MenuItemBase* m_topItems[2] = {
    dynamic_cast<MenuItemBase*>(&m_sensorMenu),
    dynamic_cast<MenuItemBase*>(&m_motorRunTime)
  };
  Array<MenuItemBase*> m_topItemsArr{m_topItems, sizeof(m_topItems) / sizeof(MenuItemBase*)};

  char m_potMenuItemLabel[LCD_COLS] = "Pot x";
  RamString m_potMenuItemLabelStr{m_potMenuItemLabel, LCD_COLS};
};

/*******************************************************************************
 ***                            Pot settings menu                             ***
 *******************************************************************************/

/**************************************************
 ***  Pot1 menu                                  ***
 **************************************************/

PotMenu pot1Menu;

/**************************************************
 ***  Pots menu                                  ***
 **************************************************/

char const potLabel2[] PROGMEM = "Pot 2";
ProgMemString potLabel2Str{potLabel2, sizeof(potLabel2)};
MenuItemText potMenuItem2{&potLabel2Str};

char const potLabel3[] PROGMEM = "Pot 3";
ProgMemString potLabel3Str{potLabel3, sizeof(potLabel3)};
MenuItemText potMenuItem3{&potLabel3Str};

// char potLabel4[] = "Pot 4";
// MenuItemText potMenuItem4{potLabel4};

// char potLabel5[] = "Pot 5";
// MenuItemText potMenuItem5{potLabel5};

// char potLabel6[] = "Pot 6";
// MenuItemText potMenuItem6{potLabel6};

MenuItemBase* potsMenuItems[] = {
  /* dynamic_cast<MenuItemBase*>(&pot1MenuList), */
  dynamic_cast<MenuItemBase*>(&pot1Menu),
  dynamic_cast<MenuItemBase*>(&potMenuItem2),
  dynamic_cast<MenuItemBase*>(&potMenuItem3)
  // dynamic_cast<MenuItemBase*>(&potMenuItem4),
  // dynamic_cast<MenuItemBase*>(&potMenuItem5),
  // dynamic_cast<MenuItemBase*>(&potMenuItem6),
};
Array<MenuItemBase*> potsMenuItemsArr{potsMenuItems, sizeof(potsMenuItems) / sizeof(MenuItemBase*)};

char const potsMenuLabel[] PROGMEM = "Pot settings";
ProgMemString potsMenuLabelStr{potsMenuLabel, sizeof(potsMenuLabel)};
MenuItemList potsMenuItemList{&potsMenuLabelStr, potsMenuItemsArr};

MoistureLevelScreen moistureScreen{};

Menu menu{&moistureScreen};

void MenuSetup(Array<FlowerPot> pots)
{
  moistureScreen = MoistureLevelScreen{pots, &potsMenuItemList};

  pot1Menu = PotMenu{&pots[0], 1};

  menu.init();
}

void MenuLoop()
{
  menu.update();
}

#endif
