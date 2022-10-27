#include "muduo/net/TimerQueue.h"
#include "muduo/net/EventLoop.h"
#include <sys/timerfd.h>
#include <unistd.h>

int createTimerfd()
{
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (fd < 0) {
        printf("Fail in timerfd_create");
    }

    return fd;
}

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_),
    timers_()
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
}

TimerQueue::~TimerQueue()
{
    ::close(timerfd_);
}

void TimerQueue::handleRead()
{
    printf("TimerQueue::handleRead()\n");    
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timerstamp when, double interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

std::vector<TimerQueue::Entry> TimerQueue::getExpried(Timerstamp now)
{
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, back_inserter(expired));
    timers_.erase(timers_.begin(), it);

    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timerstamp now)
{
    
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();  
    bool earliestChange = insert(timer);
    if(earliestChange) {
        resertTimerfd();
    }
}

bool TimerQueue::insert(Timer* timer)
{
    bool earliest = false;

    return earliest;
}

void TimerQueue::resertTimerfd()
{

}
