#include <iostream>
#include <functional>

using namespace std;

class Test
{
public:
    Test()
    {
        cout<<"construct"<<endl;
    }

    Test(const Test& t)
    {
        cout<<"copy construct"<<endl;
    }

    Test(const Test&& t)
    {
        cout<<"copy && construct"<<endl;
    }

    ~Test()
    {
        cout<<"destruct"<<endl;
    }  
};

void func(const Test& t)
{

}

int main()
{
    std::function<void()> f;
    f = std::bind(func, Test());

    f();

    return 0;
}

/*
construct
-----------------
copy construct
copy construct
copy construct
destruct
destruct
copy construct
destruct
destruct
destruct

construct
-----------------
copy construct
copy construct
copy construct
destruct
destruct
destruct
destruct
*/