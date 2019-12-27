#include "tnl/base/Logger.h"
#include "tnl/base/CurThread.h"
#include "tnl/net/EventLoopThread.h"

using namespace tnl;
using namespace tnl::net;

int main(int argc, char* argv[])
{
    EventLoopThread thr1;
    EventLoop* loop1 = thr1.startLoop();

    EventLoopThread thr2;
    EventLoop* loop2 = thr2.startLoop();

    EventLoopThread thr3;
    EventLoop* loop3 = thr3.startLoop();

    loop1->runInLoop([](){
        LOG_DEBUG("run int thread %d", CurThread::tid());
    });

    loop2->runInLoop([](){
        LOG_DEBUG("run int thread %d", CurThread::tid());
    });

    loop3->runInLoop([](){
        LOG_DEBUG("run int thread %d", CurThread::tid());
    });

    while (1)
    {
        sleep(1);
    }
}