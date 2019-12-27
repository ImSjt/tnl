#include <time.h>

#include "tnl/base/Condition.h"

using namespace tnl;

bool Condition::waitTimeout(int ms)
{
    struct timespec abstime;
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);

    abstime.tv_sec = now.tv_sec + ms/1000;
    abstime.tv_nsec = now.tv_nsec + ms%1000*1000*1000;
    
    MutexLock::UnassignGuard guard(mMutex);
    if(pthread_cond_timedwait(&mCond, mMutex.getPthreadMutex(), &abstime) == 0)
        return true;
    else
        return false;
}