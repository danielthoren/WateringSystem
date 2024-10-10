#ifndef MENU_LIB_ITEM_EDIT_INT
#define MENU_LIB_ITEM_EDIT_INT

#include "MenuItemBase.hpp"
#include "DisplayAdapter.hpp"

namespace MenuLib
{

class MenuItemEditField : public MenuItemBase
{
public:
  MenuItemEditField(char const* pFormatStr,
					uint8_t* pValue,
					uint8_t stepSize,
					uint8_t minVal,
					uint8_t maxVal)
    : m_pFormatStr{pFormatStr},
	  m_pValue{pValue},
	  m_stepSize{stepSize},
	  m_minVal{minVal},
	  m_maxVal{maxVal}
  {
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

  virtual char const* getTextLabel() override
  {
	snprintf(m_label, LCD_COLS, m_pFormatStr, *m_pValue);
    return reinterpret_cast<char const*>(m_label);
  }

protected:

  char const* m_pFormatStr{nullptr};
  char m_label[LCD_COLS];
  uint8_t* m_pValue{nullptr};
  uint8_t m_stepSize;
  uint8_t m_minVal;
  uint8_t m_maxVal;
};

}

#endif
