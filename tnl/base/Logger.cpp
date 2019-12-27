#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <stdarg.h>
#include <algorithm>
#include <iostream>

#include "tnl/base/Logger.h"
#include "tnl/base/CurThread.h"

using namespace tnl;

static __thread char timestr[64];
static __thread time_t lastSecond;

static const char* logLevelName[Logger::NUM_LOG_LEVEL] =
{
    "TRACE ",
    "DEBUG ",
    "WARN  ",
    "ERROR ",
    "FATAL "
};

static void defaultOutput(const char* msg, int len);
static void defaultFlush();

Logger::LogLevel Logger::mLogLevel = Logger::TRACE;
Logger::OutputFunc Logger::mOutput = defaultOutput;
Logger::FlushFunc Logger::mFlushFunc = defaultFlush;

static const char digits[] = "9876543210123456789";
static const char* zero = digits + 9;

static int convert(char buf[], int value)
{
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(value % 10);
        value /= 10;
        *p++ = zero[lsd];
    } while (value != 0);
    
    if (value < 0)
    {
        *p++ = '-';
    }

    *p = '\0';
    std::reverse(buf, p);
    
    return p - buf;
}

Logger::Logger(SourceFile file, int line, const char* func, LogLevel level) :
    mBaseFile(file),
    mLine(line),
    mFunc(func),
    mThisLogLevel(level)
{
    // 时间
    formatTime();

    // 线程id
    CurThread::tid();
    mBuffer.append(CurThread::tidStr(), CurThread::tidStrLen());

    // 日志级别
    mBuffer.append(logLevelName[level], 6);
}

Logger::~Logger()
{
    finish();

    mOutput(mBuffer.data(), mBuffer.length());

    if (mThisLogLevel == FATAL)
    {
        mFlushFunc();
        abort();
    }
}

void Logger::log(const char* format, ...)
{
    va_list valst;
    va_start(valst, format);
    int len = vsnprintf(mBuffer.current(), mBuffer.avail(), format, valst);
    va_end(valst);

    mBuffer.add(len);
}

void Logger::setOutput(OutputFunc func)
{
    mOutput = std::move(func);
}

void Logger::setFlush(FlushFunc func)
{
    mFlushFunc = func;
}

void Logger::formatTime()
{
    Timestamp now = Timestamp::now();
    int64_t microSecondsSinceEpoch = now.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::MicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::MicroSecondsPerSecond);

    if (seconds != lastSecond)
    {
        lastSecond = seconds;
        struct tm t;

        //::gmtime_r(&seconds, &t);
        ::localtime_r((time_t*)&seconds, &t);

        int len = snprintf(timestr, sizeof(timestr), "%4d%02d%02d %02d:%02d:%02d",
                            t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                            t.tm_hour, t.tm_min, t.tm_sec);
        assert(len == 17);
    }

    mBuffer.append(timestr, 17);

    char ms[10];
    assert(snprintf(ms, sizeof(ms), ".%06dZ ", microseconds) == 9);
    mBuffer.append(ms, 9);
}

void Logger::finish()
{
    mBuffer.append(" - ", 3);
    mBuffer.append(mBaseFile.mData, mBaseFile.mSize);
    mBuffer.append(":", 1);
    mBuffer.add(convert(mBuffer.current(), mLine));
    mBuffer.append(":", 1);
    mBuffer.append(mFunc, strlen(mFunc));
    mBuffer.append("\n", 1);
}

static void defaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
}

static void defaultFlush()
{
    printf("default flush\n");
    fflush(stdout);
}