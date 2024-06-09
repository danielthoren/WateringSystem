
#ifndef FILTER
#define FILTER

#include "CommonUtil.hpp"

template <typename T>
class LowPassFilter
{
public:
  LowPassFilter() :
    m_alpha{}
  {}

  LowPassFilter(double alpha, double startVal) :
    m_alpha{alpha},
    m_filterValue{startVal}
  {
    ASSERT(0 < alpha && alpha < 1, "Alpha must be within: 0 < a < 1 ");
    m_initialized = true;
  }

  bool isInitialized()
  {
    return m_initialized;
  }

  T filter(T value)
  {
    if (!isInitialized())
      return T{};

    m_filterValue = m_alpha * static_cast<double>(value) + (1 - m_alpha) * m_filterValue;
    return static_cast<T>(m_filterValue);
  }

  T getValue()
  {
    return m_filterValue;
  }

  private:
  double m_alpha;
  double m_filterValue{0};
  bool m_initialized{false};
};

#endif
