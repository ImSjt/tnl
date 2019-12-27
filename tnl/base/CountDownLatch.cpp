#include "tnl/base/CountDownLatch.h"

using namespace tnl;

CountDownLatch::CountDownLatch(int count) :
    mMutex(),
    mCond(mMutex),
    mCount(count)
{

}

void CountDownLatch::wait()
{
    MutexLockGuard guard(mMutex);

    while (mCount > 0)
    {
        mCond.wait();
    }
}

void CountDownLatch::countDown()
{
    MutexLockGuard guard(mMutex);

    --mCount;
    if (mCount == 0)
    {
        mCond.notify();
    }
}

int CountDownLatch::getCount()
{
    MutexLockGuard guard(mMutex);
    return mCount;
}