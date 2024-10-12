#ifndef MOISTURE_LEVEL_SCREEN
#define MOISTURE_LEVEL_SCREEN

#include "FlowerPot.hpp"
#include "Sensor.hpp"
#include "CommonUtil.hpp"

#include "MenuLib/DisplayAdapter.hpp"
#include "MenuLib/MenuItemBase.hpp"
#include "MenuLib/common.hpp"
#include "MenuLib/String.hpp"

using MenuLib::MenuItemBase;
using MenuLib::DisplayAdapter;
using MenuLib::InputEvent;
using MenuLib::IString;

class MoistureLevelScreen : public MenuItemBase
{
public:
  static constexpr uint8_t c_potsPerTextRow = 2;

  MoistureLevelScreen() = default;

  MoistureLevelScreen(Array<FlowerPot> pots, MenuItemBase* childNode)
    : m_initialized{true},
      m_pots{pots},
      m_numPotTextRows{m_pots.size() / c_potsPerTextRow},
      m_childNode{childNode}
  {}

  void init() override
  {
    m_childNode->setParent(dynamic_cast<MenuItemBase*>(this));
    m_childNode->init();
  }

  MenuItemBase* handleInput(InputEvent event) override
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
        return m_childNode;
        break;
      default:
        Serial.print("Unhandled event: ");
        Serial.println(static_cast<int>(event));
        break;
    }

    return dynamic_cast<MenuItemBase*>(this);
  }

  void draw(MenuLib::DisplayAdapter& display) override
  {
    ASSERT(m_initialized, "Not m_initialized");

    m_display = &display;

    char row1[LCD_COLS + 1] = {0};
    char row2[LCD_COLS + 1] = {0};
    char row3[LCD_COLS + 1] = {0};

    char* potRows[] = {row1, row2, row3};

    uint8_t potPos = getPotTextRow(0, row1);
    potPos = getPotTextRow(potPos, row2);
    potPos = getPotTextRow(potPos, row3);

    display.clear();
    display.setCursor(0, 0);
    display.print(potRows[m_topRow]);

    if (m_topRow + 1 < 3)
    {
      display.setCursor(0, 1);
      display.print(potRows[m_topRow + 1]);
    }
  }

  void update() override
  {
    if (m_display != nullptr && hasChanged())
    {
      draw(*m_display);
    }
  }

  IString const* getTextLabel() const override
  {
    ASSERT(false, "getTextLabel shall not be called on this type");
    return nullptr;
  }

private:

  uint8_t getPotTextRow(uint8_t startPot, char* text)
  {
    if (m_pots.size() <= startPot)
    {
      text[0] = '\0';
      return 0;
    }

    uint8_t cursorPos = 0;
    uint8_t written = 0;

    uint8_t potNum = startPot;
    for (; potNum < m_pots.size(); potNum++)
    {
      if (m_pots[potNum].isInitialized())
      {
        unsigned sensorVal = m_pots[potNum].getSensor()->getPercentageValue();
        m_displayedValues[potNum] = sensorVal;
        cursorPos += snprintf(&text[cursorPos], LCD_COLS - cursorPos, "P%u:%u%% ",
                              potNum + 1, sensorVal);
        written += 1;
        if (written == c_potsPerTextRow)
          break;
      }
    }
    text[cursorPos] = '\0';

    return potNum + 1;
  }

  bool hasChanged()
  {
    for (uint8_t i = 0; i < m_pots.size(); i++)
    {
      if (m_pots[i].isInitialized() &&
          m_pots[i].getSensor()->getPercentageValue() != m_displayedValues[i])
      {
        return true;
      }
    }
    return false;
  }

  void up()
  {
    uint8_t newTopRow = max(static_cast<int>(m_topRow) - LCD_ROWS, 0);
    if (newTopRow != m_topRow)
    {
      m_topRow = newTopRow;
    }
  }

  void down()
  {
    uint8_t newTopRow = min(m_topRow + LCD_ROWS,
                            m_numPotTextRows - (LCD_ROWS - (m_numPotTextRows % LCD_ROWS)));
    if (newTopRow != m_topRow)
    {
      m_topRow = newTopRow;
    }
  }

  uint8_t m_topRow{0};
  bool m_initialized{false};
  Array<FlowerPot> m_pots;
  uint8_t m_numPotTextRows;
  unsigned m_displayedValues[hwSupportedPotNum];

  MenuItemBase* m_childNode;
  DisplayAdapter* m_display{nullptr};
};


#endif
