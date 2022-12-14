#pragma once
#include <functional>
#include <boost/noncopyable.hpp>
#include <iostream>

class EventLoop;
class Channel : public boost::noncopyable
{
public:
    typedef std::function<void()> EventCallback;

    explicit Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();
    void setReadCallback(const EventCallback func) { readCallback_ = std::move(func); }
    void setWriteCallback(const EventCallback func) { writeCallback_ = std::move(func); }
    void setCloseCallback(const EventCallback func) { closeCallback_ = std::move(func); }
    void setErrorCallback(const EventCallback func) { errorCallback_ = std::move(func); }

    int fd() const { return fd_; } 
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    bool isWriting() const { return events_ & kReadEvent; }
    bool isReading() const { return events_ & kReadEvent; }
    bool isNonEvent() const { return events_ == kNoneEvent; }

    int index() const { return index_; }
    void set_index(int idx) { index_ = idx; }
    EventLoop* ownerLoop() const { return loop_; }
private:
    void update();
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_; 
    bool eventHandling_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

