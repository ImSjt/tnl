#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "tnl/base/MutexLock.h"

namespace tnl
{

class Condition
{
public:
    // 禁止 Condition = MutexLock 的隐式转换
    explicit Condition(MutexLock& mutex) :
        mMutex(mutex)
    {
        pthread_cond_init(&mCond, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&mCond);
    }

    void wait()
    {
        MutexLock::UnassignGuard guard(mMutex);
        pthread_cond_wait(&mCond, mMutex.getPthreadMutex());
    }

    bool waitTimeout(int ms);

    void notify()
    {
        pthread_cond_signal(&mCond);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&mCond);
    }

private:
    MutexLock& mMutex;
    pthread_cond_t mCond;
};

} // namespace tnl

#endif // _CONDITION_H_