#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "tnl/base/noncopyable.h"
#include "tnl/base/Logger.h"
#include "tnl/net/InetAddress.h"

namespace tnl
{
namespace net
{

class Socket : noncopyable
{
public:
    explicit Socket(int sockfd) :
        mSockfd(sockfd)
    { 

    }

    ~Socket();

    int fd() const { return mSockfd; }

    void bindAddress(const InetAddress& addr);

    void listen();

    int accept(InetAddress* peeraddr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);

    void setReuseAddr(bool on);

    void setReusePort(bool on);

    void setKeepAlive(bool on);

private:
    const int mSockfd;
};

} // namespace net
} // namespace tnl

#endif // _SOCKET_H_