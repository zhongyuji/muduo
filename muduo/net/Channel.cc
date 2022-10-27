#include "muduo/net/Channel.h"
#include "muduo/net/EventLoop.h"
#include <poll.h>


const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) :
    loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1)
{

}

Channel::~Channel()
{
    
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent()
{
    if(revents_ & POLLNVAL) {
        printf("Channel::handleEvent() POLLNVAL!\n");
    }

    if(revents_ & (POLLERR | POLLNVAL)) {
        if(errorCallback_) errorCallback_();
    }

}
