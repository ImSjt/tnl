#ifndef _LOG_FILE_H_
#define _LOG_FILE_H_

#include <string>

#include "tnl/base/File.h"

namespace tnl
{

const int K = 1024;
const int M = 1024 * 1024;

class LogFile
{
public:
    LogFile(std::string basename, int rollSize = 200*M);
    ~LogFile();

    void append(const char* msg, int len);
    void flush();

private:
    bool rollFile(bool force);
    void formatLogName();

private:
    std::string mBaseName;
    std::string mLogName;
    int mRollSize;
    int mAppendSize;
    File mFile;
    
};

}

#endif // _LOG_FILE_H_