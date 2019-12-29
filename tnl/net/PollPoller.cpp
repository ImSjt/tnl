#include <assert.h>
#include <poll.h>
#include <algorithm>
#include <iostream>

#include "tnl/base/Logger.h"
#include "tnl/net/PollPoller.h"
#include "tnl/net/EventLoop.h"

using namespace tnl;
using namespace tnl::net;

PollPoller::PollPoller(EventLoop* loop) :
    Poller(loop)
{

}

PollPoller::~PollPoller()
{

}

int PollPoller::poll(int timeoutMs, ChannelList& activeChannels)
{
    int num = ::poll(&*mPollFds.begin(), mPollFds.size(), timeoutMs);
    if (num > 0)
    {
        fillActiveChannels(num, activeChannels);
    }
    else if (num == 0)
    {
        // LOG_TRACE("no io ready"); 
    }
    else
    {
        LOG_WARN("poll result < 0");
    }
    
    return num;
}

void PollPoller::updateChannel(Channel* channel)
{
    // 修改通道必须在loop所在的线程中进行
    assert(mLoop->isInLoopThread());
    
    // LOG_TRACE("undate channel, fd:%d", channel->fd());
    if (channel->index() < 0) // 通道不存在于loop中
    {
        // 通道必须不存在于loop中
        assert(mChannels.find(channel->fd()) == mChannels.end());
        
        // 构建pollfd
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        mPollFds.push_back(pfd);

        // 给channel分配一个index
        int idx = static_cast<int>(mPollFds.size())-1;
        channel->setIndex(idx);

        // 将通道添加到map中
        mChannels.insert(std::make_pair(channel->fd(), channel));
    }
    else // 通道已经在loop中
    {
        // 通道必须存在于map中
        assert(mChannels.find(channel->fd()) != mChannels.end());
        assert(mChannels[channel->fd()] == channel);

        // 通道必须存在于poll列表中
        int idx = channel->index();
        assert(0 <= idx && idx < static_cast<int>(mPollFds.size()));
        struct pollfd& pfd = mPollFds[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);

        // 更新通道
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        // 让poll忽略这个channel
        if (channel->isNoneEvent())
        {
            pfd.fd = -channel->fd()-1;
        }
    }
}

void PollPoller::removeChannel(Channel* channel)
{
    // 修改通道必须在loop所在的线程中进行
    assert(mLoop->isInLoopThread());

    // LOG_TRACE("remove channel, fd:%d", channel->fd());

    // 通道必须存在map中
    assert(mChannels.find(channel->fd()) != mChannels.end());
    assert(mChannels[channel->fd()] == channel);
    
    assert(channel->isNoneEvent());
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(mPollFds.size()));
    const struct pollfd& pfd = mPollFds[idx];
    assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());

    // 将通道从map中删除
    size_t n = mChannels.erase(channel->fd());
    assert(n == 1);

    // 将通道从poll列表中删除
    if (static_cast<size_t>(idx) == mPollFds.size()-1) // 通道位于尾部
    {
        mPollFds.pop_back();
    }
    else // 位于中间
    {
        int endFd = mPollFds.back().fd;
        iter_swap(mPollFds.begin()+idx, mPollFds.end()-1); // 与最后的元素交换位置
        
        // 还原fd的值
        if(endFd < 0)
        {
            endFd = -endFd - 1;
        }

        mChannels[endFd]->setIndex(idx);
        mPollFds.pop_back();
    }
    
}

void PollPoller::fillActiveChannels(int num, ChannelList& activeChannels)
{
    for(struct pollfd pfd : mPollFds)
    {
        if(pfd.revents > 0)
        {
            --num;
            ChannelMap::const_iterator it = mChannels.find(pfd.fd);
            assert(it != mChannels.end());
            
            Channel* channel = it->second;
            assert(channel->fd() == pfd.fd);

            channel->setREvents(pfd.revents);
            activeChannels.push_back(channel);
        }

        if(num <= 0)
            break;
    }
}