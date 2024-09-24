#ifndef MENU_LIB
#define MENU_LIB

#include <LiquidCrystal_I2C.h>
#include <SimpleRotary.h>

#define LCD_ROWS 2
#define LCD_COLS 16

#include "CommonUtil.hpp"

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

  virtual void init() {}

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

  void init() override
  {
    for (MenuItemList* item : m_items)
    {
      item->setParent(this);
      item->init();
    }
  }

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
      display.setCursor(0, row);
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
      display.setCursor(LCD_COLS - 1, 0);
      display.printUpIndicator();
    }

    uint8_t botItemIndex = m_topItemIndex + LCD_ROWS - 1;
    if (botItemIndex < (m_items.size()))
    {
      display.setCursor(LCD_COLS - 1, LCD_ROWS - 1);
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
      m_encoder{2, 3, 4},
      m_display{0x27, LCD_COLS, LCD_ROWS}
  {
    ASSERT(rootItem != nullptr, "Root item may not be NULL");
  }

  void init()
  {
    m_display.init();
    update(true);
    m_currentItem->init();
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
  MenuItemBase* m_currentItem;

  SimpleRotary m_encoder;
  DisplayAdapter m_display;
};

#endif
