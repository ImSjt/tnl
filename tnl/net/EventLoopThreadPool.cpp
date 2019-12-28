#include <assert.h>

#include "tnl/net/EventLoopThreadPool.h"

using namespace tnl;
using namespace tnl::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, std::string name) :
    mBaseLoop(loop),
    mName(name),
    mStarted(false),
    mNumThreads(0),
    mNext(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    assert(!mStarted);
    assert(mBaseLoop->isInLoopThread());

    for (int i = 0; i < mNumThreads; ++i)
    {
        char buf[mName.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", mName.c_str(), i);
        
        EventLoopThread* t = new EventLoopThread(buf);
        mThreads.push_back(std::unique_ptr<EventLoopThread>(t));

        EventLoop* loop = t->startLoop();
        mLoops.push_back(loop);
    }

    mStarted = true;
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(mBaseLoop->isInLoopThread());
    assert(mStarted);

    EventLoop* loop = mBaseLoop;

    if (!mLoops.empty())
    {
        loop = mLoops[mNext];
        ++mNext;
        if (static_cast<size_t>(mNext) >= mLoops.size())
            mNext = 0;
    }

    return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
    assert(mBaseLoop->isInLoopThread());
    assert(mStarted);

    EventLoop* loop = mBaseLoop;

    if (!mLoops.empty())
    {
        loop = mLoops[hashCode % mLoops.size()];
    }

    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    assert(mBaseLoop->isInLoopThread());
    assert(mStarted);

    if (mLoops.empty())
    {
        return std::vector<EventLoop*>(1, mBaseLoop);
    }
    else
    {
        return mLoops;
    }
}
