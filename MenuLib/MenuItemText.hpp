#ifndef MENU_LIB_ITEM_TEXT
#define MENU_LIB_ITEM_TEXT

#include "MenuItemBase.hpp"
#include "DisplayAdapter.hpp"
#include "String.hpp"

namespace MenuLib
{

class MenuItemText : public MenuItemBase
{
public:
  MenuItemText(IString* textLabel)
    : m_textLabel{textLabel}
  {
    ASSERT(m_textLabel != nullptr, "m_textLabel may not be null!");
  }

  virtual IString const* getTextLabel() const override
  {
    return reinterpret_cast<IString const*>(m_textLabel);
  }

protected:
  IString* m_textLabel{nullptr};
};

}

#endif
