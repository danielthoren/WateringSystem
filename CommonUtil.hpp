#ifndef COMMON_UTIL
#define COMMON_UTIL

constexpr unsigned long msFromSec(unsigned const sec) { return sec * 1000; }
constexpr uint32_t msFromMin(uint32_t const min) { return min * 60 * 1000; }

#define ASSERT(cond, text)                      \
  do                                            \
  {                                             \
    if (!(cond))                                \
    {                                           \
      Serial.print(F("ASSERT: "));              \
      Serial.print(__func__);                   \
      Serial.print(": ");                       \
      Serial.println(F(text));                  \
      while (true) {};                          \
    }                                           \
  } while (0)

#endif
