#ifndef MENU_LIB_COMMON
#define MENU_LIB_COMMON

namespace MenuLib
{

#ifndef LCD_ROWS
#define LCD_ROWS 2
#endif

#ifndef LCD_COLS
#define LCD_COLS 16
#endif

enum class InputEvent
{
  None,
  Up,
  Down,
  Enter
};

}

#endif
