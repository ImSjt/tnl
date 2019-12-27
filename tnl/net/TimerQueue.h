#ifndef _TIMER_QUEUE_H_
#define _TIMER_QUEUE_H_

#include <vector>
#include <set>

#include "tnl/base/noncopyable.h"
#include "tnl/base/Timestamp.h"
#include "tnl/net/TimerId.h"
#include "tnl/net/Channel.h"

namespace tnl
{
namespace net
{
class EventLoop;

class TimerQueue : noncopyable
{
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(TimerCallBack cb, Timestamp when, double interval);
    void cancel(TimerId timerId);

private:
    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    bool insert(Timer* timer);

    std::vector<Timer*> getExpired(Timestamp now);
    void reset(const std::vector<Timer*>& expiredTimers, Timestamp now);

    void handleRead();

private:
    EventLoop* mLoop;
    const int mTimerfd;
    Channel mTimerChannel;

    using Entry = std::pair<Timestamp, Timer*>;
    using TimerList = std::set<Entry>;

    TimerList mTimers;
    bool mHandlingTimer;
    std::set<Timer*> mCancelTimers;
};

} // namespace net
} // namespace tnl

#endif // _TIMER_QUEUE_H_