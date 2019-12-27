#ifndef _COUNT_DOWN_LATCH_H_
#define _COUNT_DOWN_LATCH_H_

#include "tnl/base/Condition.h"
#include "tnl/base/MutexLock.h"

namespace tnl
{

class CountDownLatch
{
public:
    explicit CountDownLatch(int count);

    void wait();
    void countDown();
    int getCount();

private:
    MutexLock mMutex;
    Condition mCond;
    int mCount;
};

} // namespace tnl

#endif // _COUNT_DOWN_LATCH_H_