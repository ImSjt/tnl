#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>

#include "tnl/base/noncopyable.h"

namespace tnl
{
namespace net
{

class EventLoop;

class Channel : noncopyable 
{
public:
    using EventCallback = std::function<void()>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();
    void setReadCallback(EventCallback cb)
    { mReadCallback = std::move(cb); }
    void setWriteCallback(EventCallback cb)
    { mWriteCallback = std::move(cb); }
    void setCloseCallback(EventCallback cb)
    { mCloseCallback = std::move(cb); }
    void setErrorCallback(EventCallback cb)
    { mErrorCallback = std::move(cb); }

    int events() const { return mEvents; }
    void setREvents(int evt) { mREvents = evt; }
    
    bool isNoneEvent() const { return mEvents == mNoneEvent; }
    void enableReading() { mEvents |= mReadEvent; update(); }
    void disableReading() { mEvents &= ~mReadEvent; update(); }
    void enableWriting() { mEvents |= mWriteEvent; update(); }
    void disableWriting() { mEvents &= ~mWriteEvent; update(); }
    void disableAll() { mEvents = mNoneEvent; update(); }
    bool isWriting() const { return mEvents & mWriteEvent; }
    bool isReading() const { return mEvents & mReadEvent; }

    int fd() const { return mFd; }    
    int index() const { return mIndex; }
    void setIndex(int index) { mIndex = index; }
    EventLoop* ownerLoop() { return mLoop; }

    void remove();

private:
    void update();

private:
    EventLoop* const mLoop;
    const int mFd;  // 对应的文件描述符

    int mEvents;     // 监听的事件
    int mREvents;    // 准备就绪的事件

    int mIndex;     // 在Poller中使用

    bool mEventHandling; // 是否正在处理事件
    bool mAddToLoop;     // 是否被添加到loop中

    // 回调函数
    EventCallback mReadCallback;
    EventCallback mWriteCallback;
    EventCallback mCloseCallback;
    EventCallback mErrorCallback;

    static const int mNoneEvent;
    static const int mReadEvent;
    static const int mWriteEvent;

};

} // namespace net
} // namespace tnl

#endif // _CHANNEL_H_