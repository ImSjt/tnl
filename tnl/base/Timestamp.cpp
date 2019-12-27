#include <stdio.h>
#include <sys/time.h>

#include "tnl/base/Timestamp.h"

using namespace tnl;

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;

    return Timestamp(seconds * MicroSecondsPerSecond + tv.tv_usec);
}