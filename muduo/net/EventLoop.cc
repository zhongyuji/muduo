#include "muduo/net/EventLoop.h"
#include "muduo/net/Poller.h"
#include "muduo/net/Channel.h"
#include "muduo/net/TimerQueue.h"
#include "muduo/net/SocketsOps.h"
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/eventfd.h>

int createEventfd()
{
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(fd < 0) {
        printf("eventfd_create fail!\n");
        abort();
    }
    return fd;
}

__thread EventLoop* t_loopInThisThread = nullptr;
EventLoop::EventLoop() :
    looping_(false),
    threadId_(static_cast<pid_t>(::syscall(SYS_gettid))),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this))
{
    if(!t_loopInThisThread)
    {
        t_loopInThisThread = this;
    }

    printf("EventLoop ThreaId = %d\n", threadId_);
}

EventLoop::~EventLoop() 
{
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while (!quit_) {
        activeChannels_.clear();      
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for(ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it) {
            (*it)->handleEvent();
        }
        doPendingFunctons();
    }
    
    printf("EventLoop stop.\n");
    looping_ = false;
}

void EventLoop::assertInLoopThread()
{
    if(!isInLoopThread()) 
    {
        abortNotInLoopThread();
    }
}

bool EventLoop::isInLoopThread() const
{
    return threadId_ == static_cast<pid_t>(::syscall(SYS_gettid));
}

void EventLoop::abortNotInLoopThread() const
{
    abort();    
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() 
{
    return t_loopInThisThread;
}

void EventLoop::quit() 
{
    quit_ = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}

TimerId EventLoop::runAt(Timerstamp time, TimerCallback cb)
{
    return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb)
{
    Timerstamp time(addTime(Timerstamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb)
{
   Timerstamp time(addTime(Timerstamp::now(), interval));
   return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread()) {
        cb();
    }
    else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    {
    std::unique_ptr<std::mutex> lock;
    pendingFunctors_.push_back(std::move(cb));
    }
    if(!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t s = sockets::write(wakeupFd_, &one, sizeof(one));
    if(s != sizeof(one)) {
        printf("wakeup error\n");
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t s = sockets::read(wakeupFd_, &one, sizeof(one));
    if(s != sizeof(one)) {
        printf("wakeup error\n");
    }
}

void EventLoop::doPendingFunctons()
{
    std::vector<Functor> funcorts;
    callingPendingFunctors_ = true;

    {
    std::unique_lock<std::mutex> lock(mutex_);
    funcorts.swap(pendingFunctors_);
    }

    for(const Functor& cb : funcorts) {
        cb();
    }

    callingPendingFunctors_ = false;
}
