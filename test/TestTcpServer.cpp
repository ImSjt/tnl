#include <string>

#include "tnl/base/Logger.h"
#include "tnl/net/TcpServer.h"
#include "tnl/net/EventLoop.h"
#include "tnl/net/InetAddress.h"

using namespace tnl;
using namespace tnl::net;

int main(int argc, char* argv[])
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress("0.0.0.0", 9859), "tcp");

    server.setThreadNum(2);
    server.start();

    loop.loop();

    return 0;
}