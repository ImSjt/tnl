// 日志系统的前端测试

#include "tnl/base/Logger.h"

using namespace tnl;

int main(int argc, char* argv[])
{
    LOG_TRACE("Hello world!!!");
    LOG_DEBUG("Hello world!!!");
    LOG_WARN("Hello world!!!");
    LOG_ERROR("Hello world!!!");

    for (int i = 0; i < 3; ++i)
    {
        LOG_TRACE("num: %d", i);
        LOG_DEBUG("num: %d", i);
        LOG_WARN("num: %d", i);
        LOG_ERROR("num: %d", i);
    }

    Logger::setLogLevel(Logger::WARN);

    for (int i = 0; i < 3; ++i)
    {
        LOG_TRACE("num: %d", i);
        LOG_DEBUG("num: %d", i);
        LOG_WARN("num: %d", i);
        LOG_ERROR("num: %d", i);
    }

    return 0;
}