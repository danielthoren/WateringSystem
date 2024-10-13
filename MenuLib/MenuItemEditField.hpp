#ifndef MENU_LIB_ITEM_EDIT_INT
#define MENU_LIB_ITEM_EDIT_INT

#include "MenuItemBase.hpp"
#include "DisplayAdapter.hpp"
#include "String.hpp"

namespace MenuLib
{

class MenuItemEditField : public MenuItemBase
{
public:
  MenuItemEditField(IString const* pFormatStr,
					uint8_t* pValue,
					uint8_t stepSize,
					uint8_t minVal,
					uint8_t maxVal)
    : m_pFormatStr{pFormatStr},
      m_labelStr{m_label, LCD_COLS},
	  m_pValue{pValue},
	  m_stepSize{stepSize},
	  m_minVal{minVal},
	  m_maxVal{maxVal}
  {}

  virtual void init() override
  {
    MenuItemBase::init();

    ASSERT(m_pFormatStr != nullptr, "m_textLabel may not be null!");
    ASSERT(m_pValue != nullptr, "m_textLabel may not be null!");
  }

  virtual MenuItemBase* handleInput(InputEvent event) override
  {
    switch (event)
    {
      case(InputEvent::Down):
        *m_pValue = max(*m_pValue - m_stepSize, m_minVal);
        break;
      case(InputEvent::Up):
        *m_pValue = min(*m_pValue + m_stepSize, m_maxVal);
        break;
      case(InputEvent::Enter):
        return m_parent;
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
    m_parent->draw(display);
  }

  virtual IString const* getTextLabel() const override
  {
    char formatStrCpy[LCD_COLS];
    m_pFormatStr->getCopy(formatStrCpy, LCD_COLS);
	snprintf(m_label, LCD_COLS, formatStrCpy, *m_pValue);

    m_labelStr = RamString{m_label, LCD_COLS};

    return static_cast<IString const*>(&m_labelStr);
  }

protected:

  IString const* m_pFormatStr{nullptr};
  char m_label[LCD_COLS];
  RamString m_labelStr;

  uint8_t* m_pValue{nullptr};
  uint8_t m_stepSize;
  uint8_t m_minVal;
  uint8_t m_maxVal;
};

}

#endif
