#ifndef _EVENT_LOOP_THREAD_H_
#define _EVENT_LOOP_THREAD_H_

#include <string>

#include "tnl/base/noncopyable.h"
#include "tnl/base/Thread.h"
#include "tnl/net/EventLoop.h"

namespace tnl
{
namespace net
{

class EventLoopThread : noncopyable
{
public:
    EventLoopThread(std::string name = std::string());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

private:
    EventLoop* mLoop;
    Thread mThread;
    MutexLock mLock;
    Condition mCond;

};

} // namespace net
} // namespace tnl

#endif // _EVENT_LOOP_THREAD_H_