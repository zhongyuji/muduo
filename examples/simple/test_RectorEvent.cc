#include <sys/timerfd.h>
#include "muduo/net/EventLoop.h"
#include "muduo/net/Poller.h"
#include "muduo/net/Channel.h"
#include <stdio.h>

EventLoop* g_loop;

void timeout()
{
    printf("Timeout!\n");
    g_loop->quit();
}

int main(int args, char** argv)
{
    EventLoop loop;
    g_loop = &loop;

    int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timefd);
    channel.setReadCallback(timeout);
    channel.enableReading();
    
    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timefd, 0, &howlong, NULL);

    loop.loop();

    ::close(timefd);

    return 0;
}
