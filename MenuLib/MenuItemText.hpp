#ifndef MENU_LIB_ITEM_TEXT
#define MENU_LIB_ITEM_TEXT

#include "MenuItemBase.hpp"
#include "DisplayAdapter.hpp"

namespace MenuLib
{

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

}

#endif
