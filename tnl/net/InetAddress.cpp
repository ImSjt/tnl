#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <assert.h>

#include "tnl/net/InetAddress.h"
#include "tnl/net/SocketsOps.h"

using namespace tnl;
using namespace tnl::net;

static const in_addr_t inaddrAny = INADDR_ANY;
static const in_addr_t inaddrLoopback = INADDR_LOOPBACK;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    if (ipv6)
    {
        memset(&mAddr6, 0, sizeof mAddr6);
        mAddr6.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        mAddr6.sin6_addr = ip;
        mAddr6.sin6_port = sockets::hostToNetwork16(port);
    }
    else
    {
        memset(&mAddr, 0, sizeof mAddr);
        mAddr.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? inaddrAny : inaddrLoopback;
        mAddr.sin_addr.s_addr = sockets::hostToNetwork32(ip);
        mAddr.sin_port = sockets::hostToNetwork16(port);
    }   
}

InetAddress::InetAddress(std::string ip, uint16_t port, bool ipv6)
{
    if (ipv6)
    {
        memset(&mAddr6, 0, sizeof mAddr6);
        sockets::fromIpPort(ip.c_str(), port, &mAddr6);
    }
    else
    {
        memset(&mAddr, 0, sizeof mAddr);
        sockets::fromIpPort(ip.c_str(), port, &mAddr);
    }
}

std::string InetAddress::toIp() const
{
    char buf[64] = "";
    sockets::toIp(buf, sizeof buf, getSockAddr());
    return buf;
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = "";
    sockets::toIpPort(buf, sizeof buf, getSockAddr());
    return buf;
}

uint16_t InetAddress::toPort() const
{
    return sockets::networkToHost16(portNetEndian());
}

uint32_t InetAddress::ipNetEndian() const
{
    assert(family() == AF_INET);
    return mAddr.sin_addr.s_addr;
}
