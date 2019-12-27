#ifndef _TIMER_ID_H_
#define _TIMER_ID_H_

#include "tnl/base/copyable.h"
#include "tnl/net/Timer.h"

namespace tnl
{
namespace net
{

class TimerQueue;

class TimerId : copyable
{
public:
    TimerId() :
        mTimer(NULL),
        mSequece(0)
    {   }

    TimerId(Timer* timer, int64_t sequece) :
        mTimer(timer),
        mSequece(sequece)
    {   }

private:
    friend class TimerQueue;

    Timer* mTimer;
    int64_t mSequece;
};

} // namespace net
} // namespace tnl

#endif // _TIMER_ID_H_