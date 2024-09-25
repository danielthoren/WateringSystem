#ifndef MENU_LIB_DISPLAY_ADAPTER
#define MENU_LIB_DISPLAY_ADAPTER

#include <LiquidCrystal_I2C.h>
#include "common.hpp"

namespace MenuLib
{

class DisplayAdapter {
private:
  LiquidCrystal_I2C lcd;

  byte const downArrowMap[8] = {
    0b00100,  //   *
    0b00100,  //   *
    0b00100,  //   *
    0b00100,  //   *
    0b00100,  //   *
    0b10101,  // * * *
    0b01110,  //  ***
    0b00100   //   *
  };
  static constexpr byte downArrow{0};

  byte const upArrowMap[8] = {
    0b00100,  //   *
    0b01110,  //  ***
    0b10101,  // * * *
    0b00100,  //   *
    0b00100,  //   *
    0b00100,  //   *
    0b00100,  //   *
    0b00100   //   *
  };
  static constexpr byte upArrow{1};

  static constexpr unsigned cursorIcon     = 0x7E; // →
  static constexpr unsigned editCursorIcon = 0x7F; // ←


public:
  DisplayAdapter(uint8_t lcd_addr, uint8_t cols, uint8_t rows)
    : lcd(lcd_addr, cols, rows)
  {}

  void init()
  {
    lcd.init();
    lcd.backlight();
    clear();

    lcd.createChar(upArrow, upArrowMap);
    lcd.createChar(downArrow, downArrowMap);
  }


  void clear()
  {
    lcd.clear();
  }

  void setCursor(int col, int row)
  {
    ASSERT(row >= 0 && row < LCD_ROWS && col >= 0 && col < LCD_COLS, "Position out of bounds!");
    lcd.setCursor(col, row);
  }

  void print(const String& text) { lcd.print(text); }

  void printDownIndicator() { lcd.write(downArrow); }
  void printUpIndicator  () { lcd.write(upArrow); }

  void printCursor    () { lcd.write(cursorIcon); }
  void printEditCursor() { lcd.write(cursorIcon); }

  // Turn the backlight on or off
  void setBacklight(bool state)
  {
    if (state) {
      lcd.backlight();
    } else {
      lcd.noBacklight();
    }
  }

  // Turn off the display
  void displayOff()
  {
    lcd.noDisplay();
  }

  // Turn on the display
  void displayOn()
  {
    lcd.display();
  }
};

}

#endif
