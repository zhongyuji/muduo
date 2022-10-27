#pragma once
#include <vector>
#include <map>
#include <aio.h>
#include <boost/noncopyable.hpp>
#include "muduo/base/Timerstamp.h"

struct pollfd;
class EventLoop;
class Channel;
class Poller : public boost::noncopyable
{
public:
    typedef std::vector<Channel* > ChannelList;

    explicit Poller(EventLoop* loop);
    ~Poller();

    Timerstamp poll(int timeoutMs, ChannelList* activeChannels);
    void updateChannel(Channel* channel);
    void assertInLoopThread();
private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, Channel* > ChannelMap;

    EventLoop* ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};

