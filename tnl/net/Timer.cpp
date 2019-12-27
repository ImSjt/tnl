#include "tnl/net/Timer.h"

using namespace tnl;
using namespace tnl::net;

std::atomic<int64_t> Timer::mNumCreated;

void Timer::restart(Timestamp now)
{
    if (mRepeat)
    {
        mExpiration = addTime(now, mInterval);
    }
    else
    {
        mExpiration = Timestamp();
    }
}
