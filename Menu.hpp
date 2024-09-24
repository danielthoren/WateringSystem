#ifndef MENU
#define MENU

#include "MenuLib.hpp"
#include "MoistureLevelScreen.hpp"

/*******************************************************************************
 ***                            Pot settings menu                             ***
 *******************************************************************************/

/**************************************************
 ***  Pot1 menu                                  ***
 **************************************************/

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
  dynamic_cast<MenuItemBase*>(&pot1MenuList),
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

void MenuSetup(Array<FlowerPot> pots) {
  moistureScreen = MoistureLevelScreen{pots, &potsMenuItemList};
  menu.init();
}

void MenuLoop() {

  menu.update();

  // uint8_t rotation = encoder.rotate();
  // if (rotation == 2)
  //   moistureScreen.up();
  // else if (rotation == 1)
  //   moistureScreen.down();

  // moistureScreen.update();
}

#endif
