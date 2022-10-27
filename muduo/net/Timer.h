#pragma once
#include "muduo/base/noncopyable.h"
#include "muduo/base/Timerstamp.h"
#include <functional>
#include <atomic>

class Timer : noncopyable
{
 public:
    typedef std::function<void()> TimerCallback;
    Timer(TimerCallback cb, Timerstamp when, double interval)
        : callback_(std::move(cb)),
        expiration_(when),
        interval_(interval),
        repeat_(interval > 0.0),
        sequence_(s_numCreated_)
    { }

    void run() const
    {
        callback_();
    }

    Timerstamp expiration() const  { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const { return sequence_; }

    void restart(Timerstamp now);

    static int64_t numCreated() { return s_numCreated_; }

private:
    const TimerCallback callback_;
    Timerstamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;

    static std::atomic<int64_t> s_numCreated_;
};
