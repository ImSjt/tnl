#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <functional>
#include <string>
#include <atomic>

#include "tnl/base/CountDownLatch.h"

namespace tnl
{

class Thread
{
public:
    using ThreadFunc = std::function<void()>;

    Thread(ThreadFunc func, std::string name = std::string());
    ~Thread();

    void start();
    int join();

    pid_t tid() const { return mTId; }
    const std::string& name() const { return mName; }

private:
    static int getNextIndex() { return mNextIndex++; }
    void setDefaultName();

private:
    bool mStarted;
    bool mJoined;
    pthread_t mThreadId;
    pid_t mTId;
    ThreadFunc mFunc;
    std::string mName;

    CountDownLatch mCountDownLatch;

    static std::atomic<int> mNextIndex;
};

} // namespace tnl

#endif // _THREAD_H_