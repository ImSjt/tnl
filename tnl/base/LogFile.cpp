#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "tnl/base/LogFile.h"
#include "tnl/base/Timestamp.h"

using namespace tnl;

LogFile::LogFile(std::string basename, int rollSize) :
    mBaseName(std::move(basename)),
    mRollSize(rollSize),
    mAppendSize(0)
{
    assert(mBaseName.find("/") == std::string::npos);

    rollFile(true);
}

LogFile::~LogFile()
{

}

void LogFile::append(const char* msg, int len)
{
    int ret = mFile.write(msg, len);
    if (ret < 0)
    {
        fprintf(stderr, "log append failure\n");
        return;
    }

    mAppendSize += ret;
    rollFile(false);
}

void LogFile::flush()
{
    if (mFile.flush() != true)
    {
        fprintf(stderr, "log flush failure\n");
    }
}

bool LogFile::rollFile(bool force)
{
    if (!force)
    {
        if (mAppendSize >= mRollSize)
        {
            formatLogName();
            if (mFile.reset(mLogName) != true)
            {
                fprintf(stderr, "reset file %s failure\n", mLogName.c_str());
                return false;
            }

            mAppendSize = 0;
        }
    }
    else
    {
        formatLogName();
        if (mFile.reset(mLogName) != true)
        {
            fprintf(stderr, "reset file %s failure\n", mLogName.c_str());
            return false;
        }

        mAppendSize = 0;
    }

    return true;
}

void LogFile::formatLogName()
{
    // Timestamp now = Timestamp::now();
    // time_t seconds =  now.secondsSinceEpoch();
    
    Timestamp now = Timestamp::now();
    time_t seconds = now.secondsSinceEpoch();


    struct tm t;
    ::localtime_r(&seconds, &t);

    char buf[32];
    snprintf(buf, sizeof buf, "-%4d%02d%02d-%02d:%02d:%02d.log",
                t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec);

    mLogName = std::string("/tmp/") + mBaseName + std::string(buf);
    
    fprintf(stderr, "log: %s\n", mLogName.c_str());
}