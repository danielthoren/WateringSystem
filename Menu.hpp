#ifndef MENU
#define MENU

#define LCD_ROWS 2
#define LCD_COLS 16

#include <SimpleRotary.h>
#include <LiquidCrystal_I2C.h>

#include "MoistureLevelScreen.hpp"

// Construct the display adapter
LiquidCrystal_I2C lcd{0x27, LCD_COLS, LCD_ROWS};
SimpleRotary encoder(2, 3, 4);

MoistureLevelScreen moistureScreen;

void MenuSetup(Array<FlowerPot> pots)
{
  lcd.init();
  lcd.clear();
  lcd.backlight();

  moistureScreen = MoistureLevelScreen{pots, &lcd};
  moistureScreen.show();
}

void MenuLoop() {
  uint8_t rotation = encoder.rotate();
  if (rotation == 1)
    moistureScreen.up();
  else if (rotation == 2)
    moistureScreen.down();

  moistureScreen.update();
}

#endif
