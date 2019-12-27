#include <assert.h>
#include <poll.h>

#include "tnl/net/Channel.h"
#include "tnl/net/EventLoop.h"

using namespace tnl;
using namespace tnl::net;

const int Channel::mNoneEvent = 0;
const int Channel::mReadEvent = POLLIN | POLLPRI;
const int Channel::mWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) :
    mLoop(loop),
    mFd(fd),
    mEvents(0),
    mREvents(0),
    mIndex(-1),
    mEventHandling(false),
    mAddToLoop(false)
{

}

Channel::~Channel()
{
    // 释放通道的时候，通道必须不在处理事件，必须不存在loop中 
    assert(!mEventHandling);
    assert(!mAddToLoop);

    if (mLoop->isInLoopThread())
    {
        // 当前channel必须不存在与loop中
        assert(!mLoop->hasChannel(this));
    }
}

void Channel::handleEvent()
{
    mEventHandling = true;

    if ((mREvents & POLLHUP) && !(mREvents & POLLIN))
    {
        if(mCloseCallback) mCloseCallback();
    }

    if (mREvents & POLLNVAL)
    {
        //LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLNVAL";
    }

    if (mREvents & (POLLERR | POLLNVAL))
    {
        if (mErrorCallback) mErrorCallback();
    }
    if (mREvents & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (mReadCallback) mReadCallback();
    }
    if (mREvents & POLLOUT)
    {
        if (mWriteCallback) mWriteCallback();
    }

    mEventHandling = false;
}

void Channel::remove()
{
    assert(isNoneEvent());
    mAddToLoop = false;
    mLoop->removeChannel(this);
}

void Channel::update()
{
    mAddToLoop = true;
    mLoop->updateChannel(this);
}