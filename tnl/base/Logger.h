#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string.h>
#include <functional>

#include "tnl/base/noncopyable.h"
#include "tnl/base/FixedBuffer.h"
#include "tnl/base/Timestamp.h"

namespace tnl
{
#define LOG_TRACE(format, ...) \
    if (Logger::TRACE >= Logger::logLevel()) \
        Logger(__FILE__, __LINE__, __FUNCTION__, Logger::TRACE).log(format, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) \
    if(Logger::DEBUG >= Logger::logLevel()) \
        Logger(__FILE__, __LINE__, __FUNCTION__, Logger::DEBUG).log(format, ##__VA_ARGS__)

#define LOG_WARN(format, ...) \
    if(Logger::WARN >= Logger::logLevel()) \
        Logger(__FILE__, __LINE__, __FUNCTION__, Logger::WARN).log(format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
    if (Logger::ERROR >= Logger::logLevel()) \
        Logger(__FILE__, __LINE__, __FUNCTION__, Logger::ERROR).log(format, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) \
    if (Logger::FATAL >= Logger::logLevel()) \
        Logger(__FILE__, __LINE__, __FUNCTION__, Logger::FATAL).log(format, ##__VA_ARGS__)



class Logger : noncopyable
{
public:
    using Buffer = FixedBuffer<SmallBuffer>;
    using OutputFunc = std::function<void(const char* msg, int len)>;
    using FlushFunc = std::function<void()>;

    enum LogLevel
    {
        TRACE,
        DEBUG,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVEL
    };

    struct SourceFile
    {
    public:
        template<int N> // 为了在编译器直到字符串的长度
        SourceFile(const char (&arr)[N]) :
            mData(arr),
            mSize(N-1)
        {
            const char* slash = strrchr(mData, '/');
            if (slash)
            {
                mData = slash + 1;
                mSize -= static_cast<int>(mData - arr);
            }
        }

        const char* mData;
        int mSize;
    };

    Logger(SourceFile file, int line, const char* func, LogLevel level);
    ~Logger();

    void log(const char* format, ...);

    static void setLogLevel(LogLevel level) { mLogLevel = level; }
    static LogLevel logLevel() { return mLogLevel; }

    // 设置日志信息输出通道，默认是标准输出
    static void setOutput(OutputFunc func);
    static void setFlush(FlushFunc func);

private:
    void formatTime();
    void finish();

private:
    SourceFile mBaseFile;
    int mLine;
    const char* mFunc;
    LogLevel mThisLogLevel;

    Buffer mBuffer;

    static LogLevel mLogLevel;
    static OutputFunc mOutput;
    static FlushFunc mFlushFunc;
};

} // namespace tnl

#endif // _LOGGER_H_