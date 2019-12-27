#include "tnl/net/EventLoopThread.h"

using namespace tnl;
using namespace tnl::net;

EventLoopThread::EventLoopThread(std::string name) :
    mLoop(NULL),
    mThread(std::bind(&EventLoopThread::threadFunc, this), name),
    mLock(),
    mCond(mLock)
{

}

EventLoopThread::~EventLoopThread()
{
    if (mLoop)
    {
        mLoop->quit();
        mThread.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    mThread.start();

    EventLoop* loop;
    {
        MutexLockGuard guard(mLock);
        while (mLoop == NULL)
        {
            mCond.wait();
        }
        loop = mLoop;
    }
    
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        MutexLockGuard guard(mLock);
        mLoop = &loop;
        mCond.notify();
    }

    loop.loop();
}
