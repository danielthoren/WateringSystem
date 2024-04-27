
#ifndef SENSOR
#define SENSOR

#include "CommonUtil.hpp"

class TimePoint
{
public:

  static TimePoint Now()
  {
    return TimePoint{millis()};
  }

  TimePoint(unsigned long const& time) :
    m_timePointMs{time}
  {}

  bool operator<(TimePoint const& other)
  {
    return m_timePointMs < other.m_timePointMs;
  }

  inline bool operator> (TimePoint const& other) { return *this < other; }
  inline bool operator<=(TimePoint const& other) { return !(*this > other); }
  inline bool operator>=(TimePoint const& other) { return !(*this < other); }

  bool operator==(TimePoint const& other)
  {
    return m_timePointMs == other.m_timePointMs;
  }

  bool operator!=(TimePoint const& other) { return !(*this == other); }

  TimePoint& operator+=(TimePoint const& other)
  {
    m_timePointMs += other.m_timePointMs;
    return *this;
  }

  TimePoint& operator-=(TimePoint const& other)
  {
    m_timePointMs -= other.m_timePointMs;
    return *this;
  }

  TimePoint operator+(TimePoint other)
  {
    other += *this;
    return other;
  }

  TimePoint operator-(TimePoint other)
  {
    other -= *this;
    return other;
  }

  unsigned long getTime()
  {
    return m_timePointMs;
  }

private:
  unsigned long m_timePointMs{};
};



#endif
