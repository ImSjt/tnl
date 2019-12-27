#include <iostream>
#include <vector>

int* func()
{
    int* const p = NULL;
    return p;
}

struct A
{
    int a;
    int b;
};

int main()
{
    int a;
    long c;
    short* b;
    float f;

    b = reinterpret_cast<short*>(a);

    return 0;
}