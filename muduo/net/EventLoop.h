#pragma once
#include <iostream>
#include <boost/any.hpp>
#include <atomic>
#include <pthread.h>
#include <vector>
#include <memory>
#include <mutex>
#include "muduo/base/noncopyable.h"
#include "muduo/net/Callbacks.h"
#include "muduo/base/Timerstamp.h"
#include "muduo/net/TimerQueue.h"
#include "muduo/net/TimerId.h"
const int kPollTimeMs = 10000;

class Poller;
class Channel;
class EventLoop : public noncopyable
{
public:
    typedef std::function<void()> Functor;
    explicit EventLoop();
    ~EventLoop();

    void loop();
    void assertInLoopThread();
    bool isInLoopThread() const;
    void quit();
    void updateChannel(Channel* channel);
    
    TimerId runAt(Timerstamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double interval, TimerCallback cb);
    
    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);
    void wakeup();
    void handleRead();
    void doPendingFunctons();

    static EventLoop* getEventLoopOfCurrentThread();
private:
    void abortNotInLoopThread() const;

    typedef std::vector<Channel* > ChannelList;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> callingPendingFunctors_;
    const pid_t threadId_;
    Timerstamp pollReturnTime_;
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;

    std::unique_ptr<TimerQueue> timerQueue_;
    mutable std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;
};

