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
  {
    ASSERT(m_textLabel != nullptr, "m_textLabel may not be null!");
  }

  virtual char const* getTextLabel() override
  {
    return reinterpret_cast<char const*>(m_textLabel);
  }

protected:
  char* m_textLabel{nullptr};
};

}

#endif
