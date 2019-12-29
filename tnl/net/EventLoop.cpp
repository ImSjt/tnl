#include <assert.h>
#include <stdint.h>
#include <sys/eventfd.h>
#include <type_traits>
#include <functional>
#include <algorithm>

#include "tnl/base/CurThread.h"
#include "tnl/base/Logger.h"
#include "tnl/net/EventLoop.h"
#include "tnl/net/SocketsOps.h"
#include "tnl/net/PollPoller.h"

using namespace tnl;
using namespace tnl::net;

static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

static int createEventfd()
{
    // 创建一个非阻塞的文件描述符
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    assert(evtfd > 0);

    return evtfd;
}

EventLoop::EventLoop() :
    mThreadId(CurThread::tid()),
    mLoop(false),
    mPoller(new PollPoller(this)),
    mWakeupFd(createEventfd()),
    mWakeupChannel(new Channel(this, mWakeupFd)),
    mEventHandling(false),
    mCurChannel(NULL),
    mLock(),
    mCallingPendingFunctors(false),
    mTimerQueue(new TimerQueue(this))
{
    mWakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    mWakeupChannel->enableReading();
}

EventLoop::~EventLoop()
{
    mWakeupChannel->disableAll();
    mWakeupChannel->remove();
    ::close(mWakeupFd);
}

void EventLoop::loop()
{
    assert(!mLoop);

    mLoop = true;

    while (mLoop)
    {
        mActiveChannels.clear();
        int num = mPoller->poll(mPollTimeMs, mActiveChannels);
        if (num < 0)
        {
            LOG_WARN("poll result < 0");
        }
        else if (num == 0)
        {
            // LOG_TRACE("no channel ready");
        }
        else
        {
            // LOG_TRACE("handle %d channel", mActiveChannels.size());

            mEventHandling = true;

            for (Channel* channel : mActiveChannels)
            {
                mCurChannel = channel;
                channel->handleEvent();
            }
            
            mCurChannel = nullptr;
            mEventHandling = false;
        }

        doPendingFunctors();
    }

    mLoop = false;
}

void EventLoop::quit()
{
    mLoop = false;

    // 如果该操作不在loop中执行，那么就唤醒loop
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(mWakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("writes %d bytes instead of 8", n);
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this); // 确保channel的修改是在同一个loop中
    assert(isInLoopThread());             // 对channel的操作必须在它所在的loop线程中

    mPoller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this); // 确保channel的修改是在同一个loop中
    assert(isInLoopThread());             // 对channel的操作必须在它所在的loop线程中

    // 如果正在处理事件，那么当前移除的通道必须是自己或者不在准备就绪的通道中
    if (mEventHandling)
    {
        assert(mCurChannel == channel ||
            std::find(mActiveChannels.begin(), mActiveChannels.end(), channel) == mActiveChannels.end());
    }

    mPoller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this); // 确保channel的修改是在同一个loop中
    assert(isInLoopThread());             // 对channel的操作必须在它所在的loop线程中

    return mPoller->hasChannel(channel);  // 返回结果
}

void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread()) // 如果当前线程为loop对应的线程，那么就直接执行
    {
        cb();
    }
    else // 否则，放入队列中
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    MutexLockGuard guard(mLock);

    mPendingFunctors.push_back(std::move(cb));

    if (!isInLoopThread() || mCallingPendingFunctors)
    {
        wakeup();
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(mWakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("reads %n bytes instead of 8", n);
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    mCallingPendingFunctors = true;

    {
        MutexLockGuard guard(mLock);
        functors.swap(mPendingFunctors);
    }

    for (Functor& cb : functors)
    {
        cb();
    }

    mCallingPendingFunctors = false;
}

TimerId EventLoop::runAt(Timestamp timestamp, TimerCallBack cb)
{
    return mTimerQueue->addTimer(std::move(cb), timestamp, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallBack cb)
{
    Timestamp time(Timestamp::now());
    time = addTime(time, delay);
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallBack cb)
{
    Timestamp time(Timestamp::now());
    time = addTime(time, interval);

    return mTimerQueue->addTimer(std::move(cb), time, interval);
}

void EventLoop::cancelTimer(TimerId timerId)
{
    mTimerQueue->cancel(timerId);
}