#pragma once
#include "muduo/net/Callbacks.h"
#include <aio.h>
#include "muduo/base/noncopyable.h"
#include "muduo/net/Channel.h"
#include "muduo/base/Timerstamp.h"
#include "muduo/net/Timer.h"
#include "muduo/net/TimerId.h"
#include <assert.h>
#include <vector>
#include <set>
#include <utility>

class EventLoop;
class TimerQueue : public noncopyable
{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();
    
    TimerId addTimer(TimerCallback cb, Timerstamp when, double interval);
    void addTimerInLoop(Timer* timer);
private:
    typedef std::pair<Timerstamp, Timer* > Entry;
    typedef std::set<Entry> TimerList;

    void handleRead();
    std::vector<Entry> getExpried(Timerstamp now);
    void reset(const std::vector<Entry>& expried, Timerstamp now);
    bool insert(Timer* timer);
    void resertTimerfd();
private:
    EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    TimerList timers_;
};
