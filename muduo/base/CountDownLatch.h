#pragma once
#include "muduo/base/noncopyable.h"
#include "muduo/base/Condition.h"
#include "muduo/base/Mutex.h"

class CountDownLatch : public noncopyable
{
public:
    explicit CountDownLatch(int count);
    
    void wait();
    void countDown();
    int getCount() const;

private:
    mutable MutexLock mutex_;
    Condition condition_ GUARDED_BY(mutex_);
    int count_ GUARDED_BY(mutex_);
};
