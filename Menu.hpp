#ifndef MENU
#define MENU

#include <LiquidCrystal_I2C.h>

#include <menu.h>
#include <menuIO/liquidCrystalOutI2C.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#include "CommonUtil.hpp"

#define MAX_DEPTH 2

int test=55;

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,EXIT("<Back")
);

serialIn serial(Serial);
MENU_INPUTS(in,&serial);

LiquidCrystal_I2C lcd(0x27,16,2);

MENU_OUTPUTS(out, MAX_DEPTH
  ,LIQUIDCRYSTAL_OUT(lcd,{0,0,16,2})
  ,NONE
);
NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);//the navigation root object


void MenuInit()
{
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();

}

void MenuUpdate()
{
  nav.poll();
}



#endif
