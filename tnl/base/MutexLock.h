#ifndef _MUTEXLOCK_H_
#define _MUTEXLOCK_H_

#include <pthread.h>
#include <assert.h>

#include "tnl/base/noncopyable.h"
#include "tnl/base/CurThread.h"

namespace tnl
{

class Condition;

class MutexLock : noncopyable 
{
public:
    MutexLock() :
        mHolder(0)
    {
        pthread_mutex_init(&mMutex, NULL);
    }

    ~MutexLock()
    {
        assert(mHolder == 0); // 释放的时候锁必须不被持有，不然会造成死锁
        pthread_mutex_destroy(&mMutex);
    }

    void lock()
    {
        pthread_mutex_lock(&mMutex);
        assignHolder();
    }

    void unlock()
    {
        unassignHolder();
        pthread_mutex_unlock(&mMutex);
    }

private:
    friend class Condition;

    pthread_mutex_t* getPthreadMutex()
    {
        return &mMutex;
    }

    void assignHolder()
    {
        mHolder = CurThread::tid();
    }

    void unassignHolder()
    {
        mHolder = 0;
    }

    class UnassignGuard : noncopyable
    {
    public:
        explicit UnassignGuard(MutexLock& mutex) :
            mMutex(mutex)
        {
            mMutex.unassignHolder();
        }

        ~UnassignGuard()
        {
            mMutex.assignHolder();
        }
    private:
        MutexLock& mMutex;
    };

private:
    pthread_mutex_t mMutex;
    pid_t mHolder;
};

// RALL
class MutexLockGuard : noncopyable
{
public:
    // 禁止 MutexLockGuard = MutexLock 的隐式转换
    explicit MutexLockGuard(MutexLock& mutex) :
        mMutex(mutex)
    {
        mMutex.lock();
    }

    ~MutexLockGuard()
    {
        mMutex.unlock();
    }

private:
    MutexLock& mMutex;

};

} // namespace tnl

#endif // _MUTEXLOCK_H_