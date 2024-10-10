#ifndef MENU
#define MENU

#include "MenuLib/MenuItemBase.hpp"
#include "MenuLib/MenuItemList.hpp"
#include "MenuLib/MenuItemText.hpp"
#include "MenuLib/MenuItemEditField.hpp"
#include "MenuLib/MenuItemCallback.hpp"
#include "MenuLib/Menu.hpp"

#include "MoistureLevelScreen.hpp"
#include "CommonUtil.hpp"

using namespace MenuLib;

class PotSensorTrigLevelItem : public MenuItemEditField
{
public:
  PotSensorTrigLevelItem()
    : MenuItemEditField{m_formatStr, &m_triggerValue, step, minVal, maxVal}
  {}

  PotSensorTrigLevelItem(ISensor* sensor)
    : MenuItemEditField{m_formatStr, &m_triggerValue, step, minVal, maxVal},
      m_sensor{sensor},
      m_initialized{true}
  {
    ASSERT(m_sensor != nullptr, "m_sensor may not be null!");
    m_triggerValue = m_sensor->getTriggerThreshold();
  }

  virtual MenuItemBase* handleInput(InputEvent event) override
  {
    ASSERT(m_initialized, "PotSensorTrigLevelItem not initialized!");

    if (event == InputEvent::Enter)
    {
      m_sensor->setTriggerThreshold(m_triggerValue);
    }

    return MenuItemEditField::handleInput(event);
  }

private:
  ISensor* m_sensor{nullptr};
  uint8_t m_triggerValue{};

  bool m_initialized{false};

  char m_formatStr[LCD_COLS] = "Trig level: %d%%";
  static constexpr uint8_t step = 10;
  static constexpr uint8_t minVal = 10;
  static constexpr uint8_t maxVal = 90;
};

class PotMenu : public MenuItemList
{
public:
  PotMenu()
    : MenuItemList{m_potMenuItemLabel, Array<MenuItemBase*>{m_topItems, 2}}
  {}

  PotMenu(FlowerPot* pot, uint8_t potNum)
    : MenuItemList{m_potMenuItemLabel, Array<MenuItemBase*>{m_topItems, 2}},
      m_pot{pot},
      m_potNum{potNum},
      m_initialized{true},
      m_trigLevel{m_pot->getSensor()}
  {
    ASSERT(m_pot != nullptr, "m_pot may not be null!");

    snprintf(static_cast<char*>(m_potMenuItemLabel), LCD_COLS, "Pot: %u", potNum);
  }

private:
  FlowerPot* m_pot;
  uint8_t m_potNum;
  bool m_initialized{false};

  /************************ Sensor menu ************************/

  PotSensorTrigLevelItem m_trigLevel{};

  char m_sensorMaxValText[LCD_COLS] = "Max val: %d";
  MenuItemText m_sensorMaxVal{m_sensorMaxValText};

  char m_sensorMinValText[LCD_COLS] = "Min val: %d";
  MenuItemText m_sensorMinVal{m_sensorMinValText};

  MenuItemBase* m_sensorItems[3] = {
    dynamic_cast<MenuItemBase*>(&m_trigLevel),
    dynamic_cast<MenuItemBase*>(&m_sensorMaxVal),
    dynamic_cast<MenuItemBase*>(&m_sensorMinVal)
  };

  char m_sensorMenuLabel[LCD_COLS] = "Sensor";
  MenuItemList m_sensorMenu{m_sensorMenuLabel, Array<MenuItemBase*>{m_sensorItems, 3}};

  /************************ Motor menu ************************/

  // char m_motorRunTimeText[LCD_COLS] = "Run time: %d";
  // MenuItemText m_motorRunTime{m_motorRunTimeText};

  // MenuItemBase* m_motorItems[1] = {
  //   dynamic_cast<MenuItemBase*>(&m_motorRunTime)
  // };

  // char m_motorMenuLabel[LCD_COLS] = "Motor";
  // MenuItemList m_motorMenu{m_motorMenuLabel, Array<MenuItemBase*>{m_motorItems, 1}};

  /************************ Top menu ************************/

  MenuItemBase* m_topItems[2] = {
    dynamic_cast<MenuItemBase*>(&m_sensorMenu),
    dynamic_cast<MenuItemBase*>(&m_sensorMinVal)
    // dynamic_cast<MenuItemBase*>(&m_motorMenu)
  };

  char m_potMenuItemLabel[LCD_COLS] = "Pot x";
};

/*******************************************************************************
 ***                            Pot settings menu                             ***
 *******************************************************************************/

/**************************************************
 ***  Pot1 menu                                  ***
 **************************************************/

PotMenu pot1Menu;

char sensorConfLabel[] = "Sensor config";
MenuItemText sensorConfMenu{sensorConfLabel};

char motorConfLabel[] = "Motor config";
MenuItemText motorConfMenu{ motorConfLabel};

MenuItemBase* pot1MenuItems[2] = {
  dynamic_cast<MenuItemBase*>(&sensorConfMenu),
  dynamic_cast<MenuItemBase*>(&motorConfMenu)
};

Array<MenuItemBase*> pot1MenuItemsArr{pot1MenuItems, 2};
char pot1MenuLabel[] = "Pot 1";
MenuItemList pot1MenuList = {pot1MenuLabel, pot1MenuItemsArr};

/**************************************************
 ***  Pots menu                                  ***
 **************************************************/

char potLabel2[] = "Pot 2";
MenuItemText potMenuItem2{potLabel2};

char potLabel3[] = "Pot 3";
MenuItemText potMenuItem3{potLabel3};

char potLabel4[] = "Pot 4";
MenuItemText potMenuItem4{potLabel4};

char potLabel5[] = "Pot 5";
MenuItemText potMenuItem5{potLabel5};

char potLabel6[] = "Pot 6";
MenuItemText potMenuItem6{potLabel6};

MenuItemBase* potsMenuItems[] = {
  /* dynamic_cast<MenuItemBase*>(&pot1MenuList), */
  dynamic_cast<MenuItemBase*>(&pot1Menu),
  dynamic_cast<MenuItemBase*>(&potMenuItem2),
  dynamic_cast<MenuItemBase*>(&potMenuItem3),
  dynamic_cast<MenuItemBase*>(&potMenuItem4),
  dynamic_cast<MenuItemBase*>(&potMenuItem5),
  dynamic_cast<MenuItemBase*>(&potMenuItem6),
};

Array<MenuItemBase*> potsMenuItemsArr{potsMenuItems, 6};
char potsMenuLabel[] = "Pot settings";
MenuItemList potsMenuItemList = {potsMenuLabel, potsMenuItemsArr};

MoistureLevelScreen moistureScreen{};

Menu menu{&moistureScreen};

void MenuSetup(Array<FlowerPot> pots)
{
  moistureScreen = MoistureLevelScreen{pots, &potsMenuItemList};

  pot1Menu = PotMenu{&pots[0], 1};

  Serial.println("before menu init");
  menu.init();
  Serial.println("MenuSetup");
}

void MenuLoop()
{
  menu.update();
}

#endif
