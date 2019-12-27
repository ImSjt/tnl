#include "tnl/base/Logger.h"
#include "tnl/net/EventLoop.h"

using namespace tnl;
using namespace tnl::net;

int main(int argc, char* argv[])
{
    EventLoop loop;

    TimerId timerId1 = loop.runEvery(2, [](){
        LOG_DEBUG("timer 1");
    });

    TimerId timerId2 = loop.runAfter(5, [](){
        LOG_DEBUG("timer 2");
    });

    loop.runAfter(4.5, [&](){
        LOG_DEBUG("cancel all timer");
        loop.cancelTimer(timerId1);
        loop.cancelTimer(timerId2);
    });

    loop.loop();

    return 0;
}