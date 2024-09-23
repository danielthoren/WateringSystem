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

// class IStringWrapper
// {
// public:
//   IStringWrapper(uint16_t size)
//     : m_size{size}
//   {}

//   virtual ~IStringWrapper() = default;

//   uint16_t size() const
//   {
//     return m_size;
//   }

//   char &operator[](uint16_t index) = 0;
//   const char &operator[](uint16_t index) const = 0;

//   // Iterators
//   virtual char *begin() = 0;
//   virtual const char *begin() const = 0;
//   virtual char *end() = 0;
//   virtual const char *end() const = 0;

//   // Comparisons
//   virtual bool operator==(const IStringWrapper &rhs) const = 0;

//   bool operator!=(const IStringWrapper &rhs) const
//   {
//     return !(*this == rhs);
//   }

// private:
//   uint16_t m_size;
// };

// class RamStringWrapper : public IStringWrapper
// {
// public:
//   RamStringWrapper(char* pStr, uint16_t size)
//     : IStringWrapper{size},
//       m_pStr{pStr}
//   {}

// private:
//   char* m_pStr{nullptr};
// };

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
  virtual ~MenuItemBase() = default;

  virtual MenuItemBase* handleInput(InputEvent event) { return m_parent; };
  virtual void update() {}

  virtual void draw(DisplayAdapter& display) {}
  virtual char const* getTextLabel() = 0;

  MenuItemBase* getParent() const
  {
    return m_parent;
  }

  void setParent(MenuItemBase* parent)
  {
    m_parent = parent;
  }

protected:
  MenuItemBase* m_parent;
};

class MenuItemText : public MenuItemBase
{
public:
  MenuItemText(char* textLabel)
    : m_textLabel{textLabel}
  {}

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

  virtual MenuItemBase* handleInput(InputEvent event) override
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
        if (m_activeItemIndex == -1) // Handle back menu
        {
          ASSERT(m_parent != nullptr, "Parent may not be null");
          return m_parent;
        }

        return m_items[m_activeItemIndex];
        break;
      default:
        Serial.print("Unhandled event: ");
        Serial.println(static_cast<int>(event));
        break;
    }

    return dynamic_cast<MenuItemBase*>(this);
  }

  virtual void draw(DisplayAdapter& display) override
  {
    display.clear();

    int8_t currItem = m_topItemIndex;
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

      if (currItem == -1)
      {
        display.print("..");
      }
      else
      {
        display.print(m_items[currItem]->getTextLabel());
      }
    }

    if (-1 < m_topItemIndex)
    {
      display.setCursor(0, LCD_COLS - 1);
      display.printUpIndicator();
    }

    uint8_t botItemIndex = m_topItemIndex + LCD_ROWS - 1;
    if (botItemIndex < (m_items.size()))
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
    m_activeItemIndex = max(static_cast<int>(m_activeItemIndex) - 1, -1);

    if (m_activeItemIndex < m_topItemIndex) // Cursor above view window
    {
      m_topItemIndex = m_activeItemIndex;
    }
  }

  void down()
  {
    m_activeItemIndex = min(static_cast<int>(m_activeItemIndex) + 1, m_items.size() - 1);

    if (m_activeItemIndex - m_topItemIndex > LCD_ROWS - 1) // Cursor below view window
    {
      m_topItemIndex = m_activeItemIndex - 1;
    }
  }

  int8_t m_topItemIndex{0};
  int8_t m_activeItemIndex{0};
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
      m_currentItem = m_currentItem->handleInput(event);
      m_currentItem->draw(m_display);
    }
  }

private:
  MenuItemBase* m_prevItem;
  MenuItemBase* m_currentItem;

  SimpleRotary m_encoder;
  DisplayAdapter m_display;
};

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

Menu menu{&potsMenuItemList};

// MoistureLevelScreen moistureScreen;

void MenuSetup(Array<FlowerPot> pots) {
  sensorConfMenu.setParent(&pot1MenuList);
  motorConfMenu.setParent(&pot1MenuList);

  pot1MenuList.setParent(&potsMenuItemList);
  potMenuItem2.setParent(&potsMenuItemList);
  potMenuItem3.setParent(&potsMenuItemList);
  potMenuItem4.setParent(&potsMenuItemList);
  potMenuItem5.setParent(&potsMenuItemList);
  potMenuItem6.setParent(&potsMenuItemList);

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
