#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <functional>

#include "tnl/base/Logger.h"
#include "tnl/net/Acceptor.h"
#include "tnl/net/SocketsOps.h"
#include "tnl/net/EventLoop.h"

using namespace tnl;
using namespace tnl::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport) :
    mLoop(loop),
    mAcceptSocket(sockets::createNonblockingOrDie(listenAddr.family())),
    mAcceptChannel(loop, mAcceptChannel.fd()),
    mListenning(false)
{
    LOG_TRACE("socket(%d),ip(%s),port(%d)",
                    mAcceptChannel.fd(), listenAddr.toIp().c_str(), listenAddr.toPort());

    mAcceptSocket.setReuseAddr(true);
    mAcceptSocket.setReusePort(reuseport);
    mAcceptSocket.bindAddress(listenAddr);
    mAcceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    mAcceptChannel.disableAll();
    mAcceptChannel.remove();
}

void Acceptor::listen()
{
    assert(mLoop->isInLoopThread());
    mListenning = true;
    mAcceptSocket.listen();
    mAcceptChannel.enableReading();
}

void Acceptor::handleRead()
{
    assert(mLoop->isInLoopThread());

    InetAddress peerAddr;
    int connfd = mAcceptSocket.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (mNewConnectionCallback)
        {
            mNewConnectionCallback(connfd, peerAddr);
        }
        else
        {
            sockets::close(connfd);
        }
    }
    else
    {
        LOG_ERROR("accept fd < 0");
    }
}
