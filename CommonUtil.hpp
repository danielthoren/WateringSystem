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

template <class T>
class Array {
public:
  using type = T;

  Array()
    : m_data{nullptr},
      m_size{0}
  {}

  Array(T* data, size_t size)
    : m_data{data},
      m_size{size}
  {}

  Array(Array<T> const& other)
  {
    *this = other;
  }

  Array<T>& operator=(Array<T> const& other)
  {
    m_data = other.m_data;
    m_size = other.m_size;
    return *this;
  }

  size_t size()
  {
    return m_size;
  }

  // Item access
  T &operator[](size_t index)
  {
    ASSERT(index < m_size, "Array index out of bounds");
    return m_data[index];
  }

  const T &operator[](size_t index) const
  {
    ASSERT(index < m_size, "Array index out of bounds");
    return m_data[index];
  }

  // Iterators
  T *begin() { return &m_data[0]; }
  const T *begin() const { return &m_data[0]; }
  T *end() { return &m_data[m_size]; }
  const T *end() const { return &m_data[m_size]; }

  // Comparisons
  bool operator==(const Array<T> &rhs) const {
    if (this == &rhs)
      return true;
    for (size_t i = 0; i < m_size; i++)
      if ((*this)[i] != rhs[i])
        return false;
    return true;
  }
  bool operator!=(const Array<T> &rhs) const {
    return !(*this == rhs);
  }

private:
  T *m_data;
  size_t m_size;
};

#define ARRAY(name, type, size)                 \
  do                                            \
  {                                             \
    type name##Data[size];                      \
    Array<type> name{name##Data, (size)};       \
  } while (0)

#endif
