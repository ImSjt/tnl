#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "tnl/net/Socket.h"
#include "tnl/net/SocketsOps.h"

using namespace tnl;
using namespace tnl::net;

Socket::~Socket()
{
    sockets::close(mSockfd);
}

void Socket::bindAddress(const InetAddress& addr)
{
    sockets::bindOrDie(mSockfd, addr.getSockAddr());
}

void Socket::listen()
{
    sockets::listenOrDie(mSockfd);
}

int Socket::accept(InetAddress* peeraddr)
{
    struct sockaddr_in6 addr;

    memset(&addr, 0, sizeof addr);
    int connfd = sockets::accept(mSockfd, &addr);
    if (connfd >= 0)
    {
        peeraddr->setSockAddrInet6(addr);
    }

    return connfd;
}

void Socket::shutdownWrite()
{
    sockets::shutdownWrite(mSockfd);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(mSockfd, IPPROTO_TCP, TCP_NODELAY,
                &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR,
                &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on)
{
    #ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(mSockfd, SOL_SOCKET, SO_REUSEPORT,
                            &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on)
    {
        LOG_ERROR("SO_REUSEPORT failed");
    }
    #else
    if (on)
    {
        LOG_ERROR("SO_REUSEPORT is not supported");
    }
    #endif
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(mSockfd, SOL_SOCKET, SO_KEEPALIVE,
                &optval, static_cast<socklen_t>(sizeof optval));
}
