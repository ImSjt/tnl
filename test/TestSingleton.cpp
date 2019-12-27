// 单例模式测试

#include <assert.h>
#include <iostream>

#include "tnl/base/Singleton.h"

using namespace tnl;

class Test
{
public:
    Test()
    {
        ++num;
        std::cout<<"Hello"<<std::endl;
    }

    ~Test()
    {
        std::cout<<"Byte"<<std::endl;
    }

    void print()
    {
        std::cout<<"This is a test "<<num<<std::endl;
    }

    static int num;

};

int Test::num;

int main(int argc, char* argv[])
{
    Singleton<Test>::instance().print();
    Singleton<Test>::instance().print();
    Singleton<Test>::instance().print();
    Singleton<Test>::instance().print();
    Singleton<Test>::instance().print();
    Singleton<Test>::instance().print();

    return 0;
}