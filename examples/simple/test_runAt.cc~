#include "muduo/net/EventLoop.h"
#include <sys/syscall.h>
#include <pthread.h>
#include "muduo/base/CurrentThread.h"
#include "muduo/base/Thread.h"
#include <string>

EventLoop* g_loop;
void print() {}
void threadFunc()
{
    printf("threadFunc threadId = %d\n", CurrentThread::tid());
    //g_loop->runAfter(1.0,print);
}

int main(int args, char** argv)
{
    EventLoop loop;
    g_loop = &loop;
    pthread_t id;
//    ::pthread_create(&id, NULL, threadFunc, NULL);
    Thread th(threadFunc);
    th.start();
    loop.loop();
    return 0;
}


