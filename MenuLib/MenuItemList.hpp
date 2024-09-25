#ifndef MENU_LIB_ITEM_LIST
#define MENU_LIB_ITEM_LIST

#include "MenuItemBase.hpp"
#include "DisplayAdapter.hpp"

#include "../CommonUtil.hpp"

namespace MenuLib
{

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

}

#endif
