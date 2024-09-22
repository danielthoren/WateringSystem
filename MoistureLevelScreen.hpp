#ifndef MOISTURE_LEVEL_SCREEN
#define MOISTURE_LEVEL_SCREEN

#include "FlowerPot.hpp"
#include "Sensor.hpp"

class MoistureLevelScreen
{
public:
  static constexpr uint8_t c_potsPerTextRow = 2;

  MoistureLevelScreen() = default;

  MoistureLevelScreen(Array<FlowerPot> pots, LiquidCrystal_I2C* lcd)
    : m_initialized{true},
      m_pots{pots},
      m_numPotTextRows{m_pots.size() / c_potsPerTextRow},
      m_lcd{lcd}
  {}

  uint8_t getPotTextRow(uint8_t startPot, char* text)
  {
    if (m_pots.size() <= startPot)
    {
      text[0] = '\0';
      return;
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
    if (m_changed)
      return m_changed;

    for (uint8_t i = 0; i < m_pots.size(); i++)
    {
      if (m_pots[i].isInitialized() &&
          m_pots[i].getSensor()->getPercentageValue() != m_displayedValues[i])
      {
        m_changed = true;
        break;
      }
    }
    return m_changed;
  }

  void update()
  {
    ASSERT(m_initialized, "Not m_initialized");

    if (!hasChanged())
      return;

    m_changed = false;

    char row1[LCD_COLS + 1];
    char row2[LCD_COLS + 1];
    char row3[LCD_COLS + 1];

    char* potRows[] = {row1, row2, row3};

    uint8_t potPos = getPotTextRow(0, row1);
    potPos = getPotTextRow(potPos, row2);
    potPos = getPotTextRow(potPos, row3);

    m_lcd->clear();
    m_lcd->setCursor(0, 0);

    m_lcd->print(potRows[m_topRow]);

    m_lcd->setCursor(0, 1);

    m_lcd->print(potRows[m_topRow + 1]);
  }

  void show()
  {
    ASSERT(m_initialized, "Not m_initialized");

    m_lcd->noCursor();
    m_lcd->backlight();
  }

  void hide()
  {
    ASSERT(m_initialized, "Not m_initialized");
  }

  void up()
  {
    uint8_t newTopRow = max(static_cast<int>(m_topRow) - LCD_ROWS, 0);
    if (newTopRow != m_topRow)
    {
      m_topRow = newTopRow;
      m_changed = true;
    }
  }

  void down()
  {
    uint8_t newTopRow = min(m_topRow + LCD_ROWS,
                            m_numPotTextRows - (LCD_ROWS - (m_numPotTextRows % LCD_ROWS)));
    if (newTopRow != m_topRow)
    {
      m_changed = true;
      m_topRow = newTopRow;
    }
  }

private:

  uint8_t m_topRow{0};
  bool m_initialized{false};
  bool m_changed{true};
  Array<FlowerPot> m_pots;
  uint8_t m_numPotTextRows;
  unsigned m_displayedValues[hwSupportedPotNum];
  LiquidCrystal_I2C* m_lcd;
};


#endif
