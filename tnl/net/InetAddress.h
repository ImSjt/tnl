#ifndef _INET_ADDRESS_H_
#define _INET_ADDRESS_H_

#include <netinet/in.h>
#include <string>

#include "tnl/base/copyable.h"
#include "tnl/net/SocketsOps.h"

namespace tnl
{
namespace net
{

class InetAddress : copyable
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
    
    InetAddress(std::string ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in& addr) :
        mAddr(addr)
    { }

    explicit InetAddress(const struct sockaddr_in6& addr) :
        mAddr6(addr)
    { }

    sa_family_t family() const { return mAddr.sin_family; }
    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&mAddr6); }
    void setSockAddrInet6(const struct sockaddr_in6& addr6) { mAddr6 = addr6; }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const { return mAddr.sin_port; }

private:
    union
    {
        struct sockaddr_in mAddr;
        struct sockaddr_in6 mAddr6;
    };
};

} // namespace net
} // namespace tnl

#endif // _INET_ADDRESS_H_