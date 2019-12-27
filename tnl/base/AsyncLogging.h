#ifndef _ASYNC_LOGGING_H_
#define _ASYNC_LOGGING_H_

#include <queue>

#include "tnl/base/noncopyable.h"
#include "tnl/base/FixedBuffer.h"
#include "tnl/base/Thread.h"
#include "tnl/base/LogFile.h"
#include "tnl/base/MutexLock.h"
#include "tnl/base/Condition.h"
#include "tnl/base/CountDownLatch.h"

namespace tnl
{

// 异步日志系统的后台
class AsyncLogging
{
public:
    using Buffer = FixedBuffer<LargeBuffer>;

    AsyncLogging();
    ~AsyncLogging();

    void append(const char* msg, int len);
    void flush();

    static void setBaseName(std::string name);

private:
    void initBuffers();
    void work();

private:
    static const int DefaultBufferNUM = 4;

    std::string mBasename;
    Thread mThread;
    bool mWork;

    Buffer mBuffers[DefaultBufferNUM];
    std::queue<Buffer*> mFreeBuffers;     // 空闲的缓存
    std::queue<Buffer*> mFlushBuffers;    // 待写入磁盘中的缓存
    Buffer* mCurBuffer;
    int mBufferNum;

    MutexLock mLock;
    Condition mCond;
    CountDownLatch mLatch;

    MutexLock mFlushLock;
    Condition mFlushCond;
    bool mFlushing;

    LogFile mLog;
};

} // namespace tnl

#endif // _ASYNC_LOGGING_H_