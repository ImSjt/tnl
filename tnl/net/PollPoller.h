#ifndef _POLL_POLLER_H_
#define _POLL_POLLER_H_

#include <vector>

#include "tnl/net/Poller.h"

struct pollfd;

namespace tnl
{
namespace net
{

class PollPoller : public Poller
{
public:
    PollPoller(EventLoop* loop);
    virtual ~PollPoller() override;

    virtual int poll(int timeoutMs, ChannelList& activeChannels) override;
    virtual void updateChannel(Channel* channel) override;
    virtual void removeChannel(Channel* channel) override;

    void fillActiveChannels(int num, ChannelList& activeChannels);

private:
    using PollFdList = std::vector<struct pollfd>;
    PollFdList mPollFds;

};

} // namespace net
} // namespace tnl

#endif // _POLL_POLLER_H_