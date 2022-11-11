#include "muduo/base/Thread.h"
#include "muduo/base/CurrentThread.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

namespace detail
{
pid_t getid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}
}//namespace detail

void afterFork()
{
    CurrentThread::t_cachedTid = 0;
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init;

struct ThreadData
{
    using ThreadFunc = Thread::ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(ThreadFunc func, const std::string& name, pid_t* tid, CountDownLatch* latch)
      : func_(std::move(func)),
        name_(name),
        tid_(tid),
        latch_(latch)
    {}

    void runInThread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = nullptr;
        CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName);
        try {
            func_();
            CurrentThread::t_threadName = "finished";
        }
        /*catch (const Exception& ex) {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.stackTrace);
            abort();
        }*/
        catch (const std::exception& ex) {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
        catch (...) {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "unkonwn exception caught in Thread %s\n",name_.c_str());
            throw;
        }
    }
};

void* startThread(void* arg)
{
    ThreadData* data = static_cast<ThreadData* >(arg);
    data->runInThread();
    delete data;
    return NULL;
}

void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0) {
        t_cachedTid = detail::getid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

std::atomic<int> Thread::numCreated_(0);
Thread::Thread(ThreadFunc cb, const std::string& name) :
    started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    name_(name),
    func_(std::move(cb)),
    latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
        pthread_detach(pthreadId_);
}

void Thread::setDefaultName()
{
    int num = numCreated_;
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread_%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    if(pthread_create(&pthreadId_, NULL, &startThread, data)) {
        started_ = false;
        delete data;
        printf("failed in phread_create!\n");
    }
    else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}
