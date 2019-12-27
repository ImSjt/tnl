#include <stdio.h>
#include <assert.h>

#include "tnl/base/File.h"

using namespace tnl;

File::File() :
    mFp(NULL)
{

}

File::File(std::string file) :
    mFile(std::move(file)),
    mFp(NULL)
{

}

File::~File()
{
    if (mFp != NULL)
    {
        close();
    }
}

bool File::open()
{
    mFp = ::fopen(mFile.c_str(), "a+");

    return mFp != NULL;
}

bool File::close()
{
    if (mFp)
    {
        flush();

        if (::fclose(mFp))
            return false;

        mFp = NULL;
    }

    return true;
}

int File::write(const char* msg, int len)
{
    assert(mFp != NULL);

    return ::fwrite(msg, 1, len, mFp);
}

bool File::flush()
{
    assert(mFp != NULL);

    return ::fflush(mFp) == 0;
}

bool File::reset(std::string file)
{
    if (mFile == file)
        return true;

    if (mFp)
    {
        if (flush() != true)
            return false;

        if (close() != true)
            return false;
    }

    mFile = std::move(file);

    open();
}
