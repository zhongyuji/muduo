#pragma once
#include "muduo/base/noncopyable.h"
#include "muduo/base/Mutex.h"
#include <pthread.h>

class Condition : public noncopyable
{
public:
    explicit Condition(MutexLock& mutex) 
        : mutex_(mutex)
    {}
    
    ~Condition()
    {
        MCHECK(pthread_cond_destroy(&pcond_));
    }

    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_);
        MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
    }

    void notify()
    {
        MCHECK(pthread_cond_signal(&pcond_));
    }

    void notifyAll()
    {
        MCHECK(pthread_cond_broadcast(&pcond_));
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};
