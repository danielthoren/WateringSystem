#ifndef MENU_LIB_STRING
#define MENU_LIB_STRING

#include <avr/pgmspace.h>

namespace MenuLib
{

class IString
{
public:
  IString(uint8_t size)
    : m_size{size}
  {}

  virtual ~IString() = default;

  uint8_t size() const
  {
    return m_size;
  }

  virtual char operator[](uint8_t index) const = 0;

  uint8_t getCopy(char* cpy, uint8_t cpySize) const
  {
    ASSERT(cpySize >= m_size, "cpySize too small");

    uint8_t i{0};
    for (; i < m_size; ++i)
    {
      cpy[i] = (*this)[i];
    }
    return i;
  }

protected:
  uint8_t m_size;
};

class RamString : public IString
{
public:
  RamString(char* pStr, uint8_t size)
    : IString{size},
      m_pStr{pStr}
  {}

  char operator[](uint8_t index) const override
  {
    ASSERT(m_pStr != nullptr, "m_pStr may not be null");
    ASSERT(index < m_size, "Index out of range");

    return m_pStr[index];
  }

  void setString(char const* pStr, uint8_t size)
  {
    ASSERT(size <= m_size, "String does not fit");

    for (uint8_t i{0}; i < size; ++i)
    {
      m_pStr[i] = pStr[i];
    }
  }

  char* getRaw()
  {
    return m_pStr;
  }

private:

  char* m_pStr;
};

class ProgMemString : public IString
{
public:
  ProgMemString(char* pStr, uint8_t size)
    : IString{size},
      m_pStr{pStr}
  {}

  char operator[](uint8_t index) const override
  {
    ASSERT(m_pStr != nullptr, "m_pStr may not be null");
    ASSERT(index < m_size, "Index out of range");

    return pgm_read_byte(&(m_pStr[index]));
  }

private:

  char* m_pStr{nullptr};
};

}

#endif
