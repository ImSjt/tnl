#include <unistd.h>
#include <assert.h>
#include <sys/timerfd.h>
#include <string.h>
#include <functional>

#include "tnl/base/Logger.h"
#include "tnl/net/TimerQueue.h"
#include "tnl/net/EventLoop.h"

using namespace tnl;
using namespace tnl::net;

static int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                            TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_FATAL("Failed in timerfd_create");
    }

    return timerfd;
}

static struct timespec converTime(Timestamp timestamp)
{
    int64_t microseconds = timestamp.microSecondsSinceEpoch()
                                - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100)
    {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::MicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::MicroSecondsPerSecond) * 1000);
    
    return ts;
}

static void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;

    memset(&newValue, 0, sizeof newValue);
    memset(&oldValue, 0, sizeof oldValue);

    newValue.it_value = converTime(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_FATAL("reset timerfd failure");
    }
}

TimerQueue::TimerQueue(EventLoop* loop) :
    mLoop(loop),
    mTimerfd(createTimerfd()),
    mTimerChannel(loop, mTimerfd),
    mTimers(),
    mHandlingTimer(false)
{
    mTimerChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    mTimerChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
    mTimerChannel.disableAll();
    mTimerChannel.remove();
    ::close(mTimerfd);

    for (const Entry& entry : mTimers)
    {
        delete entry.second;
    }
}

TimerId TimerQueue::addTimer(TimerCallBack cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    mLoop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));

    return TimerId(timer, timer->sequece());
}

void TimerQueue::cancel(TimerId timerId)
{
    mLoop->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    assert(mLoop->isInLoopThread());

    bool earliestChanged = insert(timer);

    if (earliestChanged)
    {
        resetTimerfd(mTimerfd, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    assert(mLoop->isInLoopThread());

    Timer* timer = timerId.mTimer;
    assert(timer);

    if (mHandlingTimer == false)
    {
        int num = mTimers.erase(Entry(timer->expiration(), timer));
        delete timer;
    }
    else // 如果在处理定时时间时删除定时器，则不能马上删除
    {
        mCancelTimers.insert(timer);
    }
}

bool TimerQueue::insert(Timer* timer)
{
    assert(mLoop->isInLoopThread());

    bool earliestChanged = false;
    TimerList::iterator it = mTimers.begin();
    Timestamp when = timer->expiration();

    if (it == mTimers.end() || when < it->first)
    {
        earliestChanged = true;
    }

    std::pair<TimerList::iterator, bool> result = mTimers.insert(Entry(when, timer));
    assert(result.second);

    return earliestChanged;
}

std::vector<Timer*> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Timer*> expired;

    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = mTimers.lower_bound(sentry); // > 

    for (TimerList::iterator it = mTimers.begin(); it != end; ++it)
    {
        expired.push_back(it->second);
    }
    mTimers.erase(mTimers.begin(), end);

    return expired;
}

void TimerQueue::reset(const std::vector<Timer*>& expiredTimers, Timestamp now)
{
    std::set<Timer*>::iterator it;

    for (Timer* timer : expiredTimers)
    {
        // 如果当前定时器在取消队列中，则删除
        if ((it = mCancelTimers.find(timer)) != mCancelTimers.end())
        {
            mCancelTimers.erase(it);
            delete timer;
        }
        else // 如果不在取消队列中
        {
            if (timer->repeat()) // 重复定时器，则重新添加
            {
                timer->restart(now);
                insert(timer);
            }
            else // 否则删除定时器
            {
                delete timer;
            }
        }
    }

    // 清除剩余的定时器
    for (std::set<Timer*>::iterator it = mCancelTimers.begin();
            it != mCancelTimers.end(); ++it)
    {
        Timer* timer = *it;
        mTimers.erase(Entry(timer->expiration(), timer));
        delete timer;
    }
    mCancelTimers.clear();

    if (!mTimers.empty())
    {
        TimerList::iterator it = mTimers.begin();
        resetTimerfd(mTimerfd, it->first);
    }
}

void TimerQueue::handleRead()
{
    assert(mLoop->isInLoopThread());

    Timestamp now(Timestamp::now());

    // 获取到期的时钟
    std::vector<Timer*> expiredTimers = getExpired(now);

    assert(mCancelTimers.empty());

    mHandlingTimer = true;

    // 处理定时时间
    for (Timer* timer : expiredTimers)
    {
        timer->run();
    }

    mHandlingTimer = false;

    reset(expiredTimers, now);
}