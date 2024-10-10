#ifndef MENU_LIB_MENU
#define MENU_LIB_MENU

#include "common.hpp"
#include "MenuItemBase.hpp"
#include "DisplayAdapter.hpp"

#include <SimpleRotary.h>

namespace MenuLib
{

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
    m_currentItem->init();
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

    if (event != InputEvent::None)
    {
      m_currentItem = m_currentItem->handleInput(event);
      m_currentItem->draw(m_display);
    }
    else if (forceRedraw)
    {
      m_currentItem->draw(m_display);
    }
  }

private:
  MenuItemBase* m_currentItem;

  SimpleRotary m_encoder;
  DisplayAdapter m_display;
};

}

#endif
