#ifndef MENU_LIB_ITEM_BASE
#define MENU_LIB_ITEM_BASE

#include "common.hpp"
#include "DisplayAdapter.hpp"

namespace MenuLib
{

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

}

#endif
