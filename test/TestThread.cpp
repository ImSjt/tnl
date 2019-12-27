// 线程类测试

#include <unistd.h>
#include <iostream>

#include "tnl/base/Thread.h"

using namespace tnl;

int main(int argc, char* argv[])
{
    Thread th0([](){
        while(1)
        {
            std::cout<<"thread0 running..."<<std::endl;
            sleep(1);
        }
    });

    Thread th1([](){
        while(1)
        {
            std::cout<<"thread1 running..."<<std::endl;
            sleep(2);
        }
    });

    Thread th2([](){
        while(1)
        {
            std::cout<<"thread2 running..."<<std::endl;
            sleep(3);
        }
    });

    th0.start();
    th1.start();
    th2.start();

    th0.join();
    th1.join();
    th2.join();

    return 0;
}