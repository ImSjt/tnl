// 测试日志文件

#include <string.h>
#include <libgen.h>

#include "tnl/base/LogFile.h"

using namespace tnl;

int main(int argc, char* argv[])
{
    char name[256] = { 0 };
    strncpy(name, argv[0], sizeof name - 1);

    LogFile file(::basename(name));

    char str[] = "Hello World\n";
    for (int i = 0; i < 10; ++i)
    {
        file.append(str, sizeof str - 1);
    }

    return 0;
}