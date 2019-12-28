#include "tnl/base/Logger.h"
#include "tnl/net/EventLoop.h"
#include "tnl/net/Acceptor.h"
#include "tnl/net/SocketsOps.h"

using namespace tnl;
using namespace tnl::net;

int main(int argc, char* argv[])
{
    Logger::setLogLevel(Logger::DEBUG);

    EventLoop loop;
    Acceptor acceptor(&loop, InetAddress(std::string("0.0.0.0"), 9859), false);

    acceptor.setNewConnectionCallback([](int sockfd, const InetAddress& addr){
        LOG_DEBUG("new connection: socket(%d),ip(%s),port(%d)",
                    sockfd, addr.toIp().c_str(), addr.toPort());
        sockets::close(sockfd);
    });

    acceptor.listen();

    loop.loop();

    return 0;
}