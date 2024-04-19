
#ifndef FILTER
#define FILTER

template <typename T>
class LowPassFilter
{
public:
  LowPassFilter(double alpha) :
    m_alpha{alpha}
  {}

  T filter(T value)
  {
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
};

#endif
