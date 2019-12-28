#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <unistd.h>
#include <memory>
#include <vector>
#include <atomic>

#include "tnl/base/noncopyable.h"
#include "tnl/base/CurThread.h"
#include "tnl/base/MutexLock.h"
#include "tnl/net/TimerQueue.h"
#include "tnl/net/Channel.h"
#include "tnl/net/Poller.h"

namespace tnl
{
namespace net
{
// one loop per thread
// Ractor模式
class EventLoop : noncopyable 
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    // 循环处理事件
    void loop();

    // 退出循环
    void quit();

    // 判断该loop是否在其对应的线程中执行
    bool isInLoopThread() const{ return mThreadId == CurThread::tid(); }

    void wakeup();  // 唤醒loop
    void updateChannel(Channel* channel); // 更新通道
    void removeChannel(Channel* channel); // 删除通道
    bool hasChannel(Channel* channel); // 判断channel在loop中

    // 多线程
    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    TimerId runAt(Timestamp timestamp, TimerCallBack cb);
    TimerId runAfter(double delay, TimerCallBack cb);
    TimerId runEvery(double interval, TimerCallBack cb);

    void cancelTimer(TimerId timerId);

private:
    // 处理wakeup channel的读
    void handleRead();

    void doPendingFunctors();

private:
    const int mThreadId; // loop所在线程的id

    std::atomic<bool> mLoop;
    std::unique_ptr<Poller> mPoller; // IO复用器

    // 用于唤醒loop
    int mWakeupFd;
    std::unique_ptr<Channel> mWakeupChannel;

    std::atomic<bool> mEventHandling;   // 当前是否正在处理事件
    Channel* mCurChannel;               // 缓存当前正在处理的通道
    Poller::ChannelList mActiveChannels; // 准备就绪的通道

    MutexLock mLock;    
    std::vector<Functor> mPendingFunctors;
    bool mCallingPendingFunctors;

    std::unique_ptr<TimerQueue> mTimerQueue;

    static const int mPollTimeMs = 10000; // poll等待的最长时间

};

} // namespace net
} // namespace tnl


#endif // _EVENT_LOOP_H_