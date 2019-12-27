#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>
#include <atomic>

#include "tnl/base/noncopyable.h"
#include "tnl/base/Timestamp.h"

namespace tnl
{
namespace net
{

using TimerCallBack = std::function<void()>;

class Timer : noncopyable
{
public:
    Timer(TimerCallBack cb, Timestamp when, double interval) :
        mCallBack(std::move(cb)),
        mExpiration(when),
        mInterval(interval),
        mRepeat(interval > 0.0),
        mSequece(mNumCreated++)
    { }

    void run() const
    {
        mCallBack();
    }

    Timestamp expiration() const  { return mExpiration; }
    bool repeat() const { return mRepeat; }

    int64_t sequece() const { return mSequece; }

    void restart(Timestamp now);

private:
    TimerCallBack mCallBack;
    Timestamp mExpiration;
    const double mInterval;
    const bool mRepeat;
    const int64_t mSequece;

    static std::atomic<int64_t> mNumCreated;
};

}
}

#endif // _TIMER_H_