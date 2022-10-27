#pragma once 
#include <boost/operators.hpp>
#include <muduo/base/copyable.h>
#include <string>
#include <sys/time.h>

class Timerstamp : public copyable , 
                  boost::less_than_comparable<Timerstamp>, 
                  boost::equality_comparable<Timerstamp>
{
public:
    Timerstamp() 
        : microSecondsSinceEpoch_(0)
    {
    }
    explicit Timerstamp(int64_t microSecondsSinceEpochArg) 
        : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    {
    }
    void swap(Timerstamp& that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }
    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;
    bool valid() const { return microSecondsSinceEpoch_ > 0; }
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    int64_t secondsSinceEpoch() const { return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }
    
    static Timerstamp now();
    static Timerstamp invalid()
    {
        return Timerstamp();
    }

    static Timerstamp formUnixTime(time_t t, int64_t microseconds)
    {
        return Timerstamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }
    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timerstamp ts1, Timerstamp ts2) 
{
    return ts1.microSecondsSinceEpoch() < ts2.microSecondsSinceEpoch();
}

inline bool operator==(Timerstamp ts1, Timerstamp ts2)
{
    return ts1.microSecondsSinceEpoch() == ts2.microSecondsSinceEpoch();
}

inline double timeDifference(const Timerstamp& hig, const Timerstamp& low)
{
    int64_t diff = hig.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff / Timerstamp::kMicroSecondsPerSecond);
}

inline Timerstamp addTime(const Timerstamp& ts, int seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timerstamp::kMicroSecondsPerSecond);
    return Timerstamp(ts.microSecondsSinceEpoch() + delta);
}
