#include "tnl/base/AsyncLogging.h"

using namespace tnl;

static char baseName[256] = {"tnl"};

AsyncLogging::AsyncLogging() :
    mBasename(baseName),
    mThread(std::bind(&AsyncLogging::work, this), "Logging"),
    mCurBuffer(NULL),
    mBufferNum(DefaultBufferNUM),
    mLock(),
    mCond(mLock),
    mFlushLock(),
    mFlushCond(mFlushLock),
    mFlushing(false),
    mLatch(1),
    mLog(baseName)
{
    initBuffers();

    mWork = true;
    mThread.start();

    // 等待后台线程启动
    mLatch.wait();
}

AsyncLogging::~AsyncLogging()
{
    mWork = false;
    mThread.join();
}

void AsyncLogging::append(const char* msg, int len)
{
    MutexLockGuard guard(mLock);

    // 如果当前没有指定缓存或者当前缓存剩余空间不足
    if (mCurBuffer == NULL || mCurBuffer->avail() < len)
    {
        // 将缓存放入写队列中
        if (mCurBuffer)
        {
            mFlushBuffers.push(mCurBuffer);
            mCurBuffer = NULL;
        }

        if (!mFreeBuffers.empty()) // 如果还剩余空闲缓存，就取出一块空闲缓存
        {
            mCurBuffer = mFreeBuffers.front();
            mFreeBuffers.pop();
        }
        else // 如果没有空闲缓存，那么就新申请一块
        {
            // 最多可以同时存在10块缓存
            if (mBufferNum <= 10)
            {
                mCurBuffer = new Buffer(mBufferNum++);
            }
        }
    }

    // 如果得到缓存，那么就将日志写入，否则丢弃
    if (mCurBuffer)
        mCurBuffer->append(msg, len);
    else
        fprintf(stdout, "log discard\n");

    // 如果写队列非空，就唤醒后台线程写
    if (!mFlushBuffers.empty())
    {
        mCond.notify();
    }
}

void AsyncLogging::flush()
{
    {
        MutexLockGuard guard(mLock);
        mFlushing = true;
        mCond.notify();
    }

    while (mFlushing)
    {
        MutexLockGuard guard(mFlushLock);
        mFlushCond.wait();
    }
}

void AsyncLogging::initBuffers()
{
    for (int i = 0; i < DefaultBufferNUM; ++i)
    {
        mBuffers[i].setIndex(i);
        mFreeBuffers.push(&mBuffers[i]);
    }
}

// 后台工作线程，负责将日志写入磁盘
void AsyncLogging::work()
{
    mLatch.countDown();
    while (mWork)
    {
        std::queue<Buffer*> tmpBuffers;
        
        // 等待flush
        {
            MutexLockGuard guard(mLock);

            while (mFlushBuffers.empty())
            {
                // 等待3s
                mCond.waitTimeout(3*1000);

                if (mCurBuffer != NULL)
                {
                    mFlushBuffers.push(mCurBuffer);
                    mCurBuffer = NULL;
                }
            }

            // 准备flush
            tmpBuffers.swap(mFlushBuffers);
        }

        while (!tmpBuffers.empty())
        {
            Buffer* buffer = tmpBuffers.front();
            tmpBuffers.pop();
            
            mLog.append(buffer->data(), buffer->length());

            {
                MutexLockGuard guard(mLock);
                if (buffer->index() >= DefaultBufferNUM)
                {
                    delete buffer;
                    --mBufferNum;
                }
                else
                {
                    buffer->reset();
                    mFreeBuffers.push(buffer);   
                }
            }
        }

        mLog.flush();

        if (mFlushing)
        {
            MutexLockGuard guard(mFlushLock);
            mFlushing = false;
            mFlushCond.notifyAll();
        }
    }
}

void AsyncLogging::setBaseName(std::string name)
{
    strcpy(baseName, name.c_str());
}