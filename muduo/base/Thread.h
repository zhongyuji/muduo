#pragma once
#include "muduo/base/noncopyable.h"
#include "muduo/base/CountDownLatch.h"
#include <pthread.h>
#include <string>
#include <functional>
#include <atomic>
#include <string>

class Thread : public noncopyable {
public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread(ThreadFunc cb, const std::string& name = std::string());
    ~Thread();

    void start();
    int join();
    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }

    static int numCreated() { return numCreated_; }
private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    std::string name_;
    ThreadFunc func_;
    CountDownLatch latch_;

    static std::atomic<int> numCreated_;
};
