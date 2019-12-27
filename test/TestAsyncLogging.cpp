#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/time.h>
#include <iostream>
#include <vector>

#include "tnl/base/AsyncLogging.h"
#include "tnl/base/Singleton.h"
#include "tnl/base/Timestamp.h"

using namespace tnl;

void becth1()
{
    char str[] = "0123456789"
                 "0123456789"
                 "0123456789"
                 "0123456789"
                 "0123456789"
                 "0123456789"
                 "0123456789"
                 "0123456789"
                 "0123456789"
                 "0123456789";
    std::vector<uint64_t> nums;
    int cnt = 0;
    nums.reserve(20);
    
    // 110M/S
    for (int t = 0; t < 11; ++t)
    {
        struct timeval start, end;

        gettimeofday(&start, NULL);

        for (uint64_t i = 0; i < 100000; ++i)
            Singleton<AsyncLogging>::instance().append(str, sizeof str - 1);

        gettimeofday(&end, NULL);

        uint64_t startus = start.tv_sec*1000*1000 + start.tv_usec;
        uint64_t endus = end.tv_sec*1000*1000 + end.tv_usec;
        uint64_t ius = endus - startus;

        uint64_t second = ius / (1000*1000);        
        uint64_t us = ius % (1000*1000);
        uint64_t ms = us / 1000;
        us = us % 1000;

        nums.push_back(second*1000*1000 + ms*1000 + us);
        ++cnt;
        std::cout<<"second:"<<second<<",ms:"<<ms<<",us:"<<us<<std::endl;

        usleep(100*1000);
    }

    uint64_t average = 0;
    for (uint64_t n : nums)
    {
        average += n;
    }
    average /= cnt;
    std::cout<<"average:"<<average<<std::endl;
}

int main(int argc, char* argv[])
{
    char name[256] = { 0 };
    strncpy(name, argv[0], sizeof name - 1);
    AsyncLogging::setBaseName(::basename(name));

    becth1();

    while (1)
    {
        sleep(1);
    }

    return 0;
}