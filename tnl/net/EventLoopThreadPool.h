#ifndef _EVENT_LOOP_THREAD_POOL_H_
#define _EVENT_LOOP_THREAD_POOL_H_

#include <memory>
#include <vector>

#include "tnl/base/noncopyable.h"
#include "tnl/net/EventLoopThread.h"

namespace tnl
{
namespace net
{

class EventLoopThreadPool : noncopyable
{
public:
    EventLoopThreadPool(EventLoop* loop, std::string name = std::string());
    ~EventLoopThreadPool();

    void setThreadNum(int num)
    { mNumThreads = num; }

    void start();

    EventLoop* getNextLoop();

    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();

    bool started() const
    { return mStarted; }

    std::string name() const
    { return mName; }

private:
    EventLoop* mBaseLoop;
    std::string mName;
    bool mStarted;
    int mNumThreads;
    int mNext;
    
    std::vector<std::unique_ptr<EventLoopThread>> mThreads;
    std::vector<EventLoop*> mLoops;
};

}
} // namespace tnl

#endif // _EVENT_LOOP_THREAD_POOL_H_