#include <assert.h>

#include "tnl/net/Poller.h"
#include "tnl/net/EventLoop.h"

using namespace tnl;
using namespace tnl::net;

Poller::Poller(EventLoop* loop) :
    mLoop(loop)
{

}

Poller::~Poller()
{

}

bool Poller::hasChannel(Channel* channel) const
{
    assert(mLoop->isInLoopThread()); // 操作loop中的数据必须在其所在的线程中
    ChannelMap::const_iterator it = mChannels.find(channel->fd());
    return it != mChannels.end();
}