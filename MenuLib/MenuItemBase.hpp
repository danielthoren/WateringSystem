#ifndef MENU_LIB_ITEM_BASE
#define MENU_LIB_ITEM_BASE

#include "common.hpp"
#include "DisplayAdapter.hpp"
#include "String.hpp"

namespace MenuLib
{

class MenuItemBase
{
public:
  MenuItemBase(MenuItemBase* m_child = nullptr)
    : m_parent{nullptr},
    m_child{m_child}
  {}

  virtual ~MenuItemBase() = default;

  virtual void init() { m_initialized = true; }

  virtual MenuItemBase* handleInput(InputEvent event)
  {
    if (event == InputEvent::Enter && m_child != nullptr)
    {
      return m_child;
    }
    return m_parent;
  }

  virtual void update() {}

  virtual void draw(DisplayAdapter& display) {}

  // TODO: Implement here and hold the label pointer in the base class? Identical funciton in almost
  // all children
  virtual IString const* getTextLabel() const = 0;

  MenuItemBase* getParent() const
  {
    return m_parent;
  }

  void setParent(MenuItemBase* parent)
  {
    m_parent = parent;
  }

protected:
  MenuItemBase* m_parent{nullptr};
  MenuItemBase* m_child{nullptr};
  bool m_initialized{false};

private:
};

}

#endif
