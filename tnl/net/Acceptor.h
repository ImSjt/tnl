#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "tnl/base/noncopyable.h"
#include "tnl/net/InetAddress.h"
#include "tnl/net/Socket.h"
#include "tnl/net/Channel.h"

namespace tnl
{
namespace net
{

class EventLoop;

class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void (int sockfd, const InetAddress&)>;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { mNewConnectionCallback = cb; }

    bool listenning() const { return mListenning; }

    void listen();

private:
    void handleRead();

private:
    EventLoop* mLoop;
    Socket mAcceptSocket;
    Channel mAcceptChannel;
    bool mListenning;
    NewConnectionCallback mNewConnectionCallback;

};

} // namespace net
} // namespace tnl

#endif // _ACCEPTOR_H_