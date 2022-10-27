#include "muduo/net/EventLoop.h"
#include "stdio.h"
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>

EventLoop* g_loop;

void *threadFunc(void *arg) 
{
    printf("threadFunc(): pid = %d, tid = %d\n",getpid(), static_cast<pid_t>(::syscall(SYS_gettid)));
    //EventLoop loop;
    g_loop->loop();
}

int main(int args, char** argv) 
{
    printf("main(): pid = %d, tid = %d\n",getpid(), static_cast<pid_t>(::syscall(SYS_gettid)));

    EventLoop loop;
    g_loop = &loop;
    pthread_t id;
    //pthread_create(&id,NULL,threadFunc,NULL);
    loop.loop();
    EventLoop loop1;
    loop1.loop();
    char *s = NULL;
    //pthread_join(id, (void**)&s);
    return 0;
}
