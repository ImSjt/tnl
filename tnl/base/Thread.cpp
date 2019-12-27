#include <assert.h>
#include <stdlib.h>
#include <iostream>

#include "tnl/base/Thread.h"

using namespace tnl;

static void* startThread(void* data);

std::atomic<int> Thread::mNextIndex;

struct ThreadData
{
    ThreadData(Thread::ThreadFunc func, const std::string& name,
                pid_t* tid, CountDownLatch* latch) :
        mFunc(std::move(func)),
        mName(name),
        mTid(tid),
        mLatch(latch)
    {

    }

    void runInThread()
    {
        *mTid = CurThread::tid();

        mLatch->countDown();

        mFunc();
    }

    Thread::ThreadFunc mFunc;
    std::string mName;
    pid_t* mTid;
    CountDownLatch* mLatch;
};

Thread::Thread(ThreadFunc func, std::string name) :
    mStarted(false),
    mJoined(false),
    mThreadId(0),
    mTId(0),
    mFunc(func),
    mName(std::move(name)),
    mCountDownLatch(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (mStarted)
    {
        pthread_detach(mThreadId);
    }
}

void Thread::start()
{
    assert(!mStarted);

    mStarted = true;

    // 启动线程
    // 线程函数中需要设置线程ID，
    ThreadData* threadData = new ThreadData(mFunc, mName, &mTId, &mCountDownLatch);
    if (pthread_create(&mThreadId, NULL, startThread, threadData))
    {
        mStarted = false;
        delete threadData;
        // log...
    }
    else
    {
        // 等待线程启动后退出
        mCountDownLatch.wait();
    }
}

int Thread::join()
{
    assert(mStarted);
    assert(!mJoined);

    mJoined = true;
    return pthread_join(mThreadId, NULL);
}

void Thread::setDefaultName()
{
    int num = getNextIndex();

    if(mName.empty())
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        mName = std::move(std::string(buf));
    }
}

static void* startThread(void* data)
{
    ThreadData* mdata = static_cast<ThreadData*>(data);
    mdata->runInThread();
    delete mdata;
}