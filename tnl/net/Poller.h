#ifndef _POLLER_H_
#define _POLLER_H_

#include <vector>
#include <map>

#include "tnl/base/noncopyable.h"
#include "tnl/net/Channel.h"

namespace tnl
{
namespace net
{

class EventLoop;

// IO 复用器
class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller();

    // 函数：poll
    // 功能：多路复用IO
    // 参数：timeoutMs：最长等待时间
    //      activeChannels：准备就绪的通道
    // 返回值：准备就绪的通道数
    virtual int poll(int timeoutMs, ChannelList& activeChannels) = 0;
    
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual bool hasChannel(Channel* channel) const;

protected:
    EventLoop* mLoop;

    using ChannelMap = std::map<int, Channel*>;
    ChannelMap mChannels;
};

} // namespace net
} // namespace tnl

#endif // _POLLER_H_