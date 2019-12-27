// event loop 测试

#include <iostream>
#include <unistd.h>

#include "tnl/net/EventLoop.h"

using namespace tnl;
using namespace tnl::net;

int main(int argc, char* argv[])
{
    EventLoop* loop = new EventLoop();
    Channel* channel = new Channel(loop, 0);

    channel->setReadCallback([channel](){
        char buf[256];
        int num = read(0, buf, sizeof buf);
        buf[num] = '\0';
        std::cout<<buf<<std::endl;

        channel->disableAll();
        channel->remove();
    });
    channel->enableReading();

    loop->loop();

    return 0;
}