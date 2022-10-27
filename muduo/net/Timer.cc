#include "muduo/net/Timer.h"

std::atomic<int64_t> Timer::s_numCreated_;

void Timer::restart(Timerstamp now)
{
  if (repeat_)
  {
    expiration_ = addTime(now, interval_);
  }
  else
  {
    expiration_ = Timerstamp::invalid();
  }
}
