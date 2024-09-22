#ifndef MENU
#define MENU

#include <LcdMenu.h>
#include <ItemList.h>
#include <interface/LiquidCrystalI2CAdapter.h>
#include <utils/commandProccesors.h>
#include <utils/RotaryNavConfig.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ROWS 2
#define LCD_COLS 16

#include "CommonUtil.hpp"
#include "MoistureLevelScreen.hpp"

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

  void setCursor(int row, int col)
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

enum class InputEvent
{
  None,
  Up,
  Down,
  Enter
};

class MenuItemBase
{
public:
  virtual MenuItemBase* handleInput(InputEvent event, MenuItemBase* parent) = 0;
  virtual void update() = 0;

  virtual void draw(DisplayAdapter& display) = 0;
  virtual char const* getTextLabel() = 0;
};

class MenuItemText : public MenuItemBase
{
public:
  MenuItemText(char* textLabel)
    : m_textLabel{textLabel}
  {}

  virtual MenuItemBase* handleInput(InputEvent event, MenuItemBase* parent) override
  {
    return parent;
  }

  virtual void update() override {}

  virtual void draw(DisplayAdapter& display) override {}

  virtual char const* getTextLabel() override
  {
    return reinterpret_cast<char const*>(m_textLabel);
  }

private:
  char* m_textLabel;
};

class MenuItemList : public MenuItemBase
{
public:
  MenuItemList(char* textLabel, Array<MenuItemBase*> items)
    : m_items{items},
      m_textLabel{textLabel}
  {}

  virtual MenuItemBase* handleInput(InputEvent event, MenuItemBase* parent) override
  {
    switch (event)
    {
      case(InputEvent::Down):
        down();
        break;
      case(InputEvent::Up):
        up();
        break;
      case(InputEvent::Enter):
        return m_items[m_activeItemIndex];
        break;
      default:
        Serial.print("Unhandled event: ");
        Serial.println(static_cast<int>(event));
        break;
    }

    return dynamic_cast<MenuItemBase*>(this);
  }

  virtual void update() override
  {

  }

  virtual void draw(DisplayAdapter& display) override
  {
    display.clear();

    uint8_t currItem = m_topItemIndex;
    for (uint8_t row = 0; row < LCD_ROWS; row++, currItem++)
    {
      display.setCursor(row, 0);
      if (currItem == m_activeItemIndex)
      {
        display.printCursor();
      }
      else
      {
        display.print(" ");
      }
      display.print(m_items[currItem]->getTextLabel());
    }

    if (0 < m_topItemIndex)
    {
      display.setCursor(0, LCD_COLS - 1);
      display.printUpIndicator();
    }

    uint8_t botItemIndex = m_topItemIndex + LCD_ROWS - 1;
    if (botItemIndex < (m_items.size() - 1))
    {
      display.setCursor(LCD_ROWS - 1, LCD_COLS - 1);
      display.printDownIndicator();
    }
  }

  virtual char const* getTextLabel() override
  {
    return reinterpret_cast<char const*>(m_textLabel);
  }

private:
  void up()
  {
    Serial.print("Up from: ");
    Serial.println(m_activeItemIndex);

    m_activeItemIndex = max(static_cast<int>(m_activeItemIndex) - 1, 0);

    if (m_activeItemIndex < m_topItemIndex) // Cursor above view window
    {
      m_topItemIndex = m_activeItemIndex;
      Serial.println("Cursor at top");
    }

    Serial.print(" to: ");
    Serial.println(m_activeItemIndex);
  }

  void down()
  {
    Serial.print("Down from: ");
    Serial.println(m_activeItemIndex);

    m_activeItemIndex = min(static_cast<int>(m_activeItemIndex) + 1, m_items.size() - 1);

    if (m_activeItemIndex - m_topItemIndex > LCD_ROWS - 1) // Cursor below view window
    {
      m_topItemIndex = m_activeItemIndex - 1;
      Serial.println("Cursor at bottom");
    }

    Serial.print(" to: ");
    Serial.println(m_activeItemIndex);
  }

  uint8_t m_topItemIndex{0};
  uint8_t m_activeItemIndex{0};
  char* m_textLabel;

  Array<MenuItemBase*> m_items;
};

class Menu
{
public:

  Menu(MenuItemBase* rootItem)
    : m_currentItem{rootItem},
      m_prevItem{rootItem},
      m_encoder{2, 3, 4},
      m_display{0x27, LCD_COLS, LCD_ROWS}
  {
    ASSERT(rootItem != nullptr, "Root item may not be NULL");
  }

  void init()
  {
    m_display.init();
    update(true);
  }

  void update(bool forceRedraw = false)
  {
    m_currentItem->update();

    InputEvent event = InputEvent::None;

    uint8_t pressType = m_encoder.push();
    uint8_t rotation = m_encoder.rotate();

    if (pressType != 0)
    {
      event = InputEvent::Enter;
    }
    else if (rotation == 1)
    {
      event = InputEvent::Down;
    }
    else if (rotation == 2)
    {
      event = InputEvent::Up;
    }

    if (event != InputEvent::None || forceRedraw)
    {
      MenuItemBase* nextItem = m_currentItem->handleInput(event, m_prevItem);
      m_currentItem->draw(m_display);
      if (nextItem != m_currentItem)
      {
        m_prevItem = m_currentItem;
        m_currentItem = nextItem;
      }
    }
  }

private:
  MenuItemBase* m_prevItem;
  MenuItemBase* m_currentItem;

  SimpleRotary m_encoder;
  DisplayAdapter m_display;
};

char itemTextStr1[] = "Item 1";
MenuItemText itemText1{itemTextStr1};

char itemTextStr2[] = "Item 2";
MenuItemText itemText2{itemTextStr2};

char itemTextStr3[] = "Item 3";
MenuItemText itemText3{itemTextStr3};

MenuItemBase* menuListItems1[] = {
  dynamic_cast<MenuItemBase*>(&itemText1),
  dynamic_cast<MenuItemBase*>(&itemText2),
  dynamic_cast<MenuItemBase*>(&itemText3),
};

Array<MenuItemBase*> menuListItemsArr1{menuListItems1, 3};
char menuItemListLabel1[] = "Menu item list 1";
MenuItemList menuItemList1{menuItemListLabel1, menuListItemsArr1};

Menu menu{&menuItemList1};

// MoistureLevelScreen moistureScreen;

void MenuSetup(Array<FlowerPot> pots) {
  // moistureScreen = MoistureLevelScreen{pots, &lcd};
  // moistureScreen.show();

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
