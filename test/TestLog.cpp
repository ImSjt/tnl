// 测试日志系统

#include <unistd.h>
#include <libgen.h>
#include <functional>

#include "tnl/base/Logger.h"
#include "tnl/base/AsyncLogging.h"
#include "tnl/base/Singleton.h"

using namespace tnl;

void output(const char* msg, int len)
{
    Singleton<AsyncLogging>::instance().append(msg, len);
}

void flush()
{
    Singleton<AsyncLogging>::instance().flush();
}

int main(int argc, char* argv[])
{
    char name[256] = { 0 };
    strncpy(name, argv[0], sizeof name - 1);
    AsyncLogging::setBaseName(::basename(name));
    Logger::setOutput(output);
    Logger::setFlush(flush);

    LOG_TRACE("Hello world");
    LOG_DEBUG("Hello world");
    LOG_WARN("Hello world");
    LOG_ERROR("Hello world");

    for (int i = 0; i < 10; ++i)
    {
        LOG_TRACE("num:%d", i);
    }

    LOG_FATAL("This is a test");

    return 0;
}