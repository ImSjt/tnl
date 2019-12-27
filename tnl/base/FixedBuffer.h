#ifndef _FIXED_BUFFER_H_
#define _FIXED_BUFFER_H_

#include <string.h>

#include "tnl/base/noncopyable.h"

namespace tnl
{

const int SmallBuffer = 4000;
const int LargeBuffer = 4000*1024;

template<int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer(int index = -1) :
        mIndex(index),
        mCur(mData)
    {
        
    }

    ~FixedBuffer()
    {
        
    }

    void append(const char* buf, int len)
    {
        if (avail() >= len)
        {
            memcpy(mCur, buf, len);
            add(len);
        }
    }

    const char* data() const { return mData; }
    int length() const { return static_cast<int>(mCur - mData); }

    char* current() { return mCur; }
    int avail() const { return static_cast<int>(end() - mCur); }
    void add(int len) { mCur += len; }

    void reset() { mCur = mData; }
    void bzero() { memset(mData, 0, sizeof(mData)); }

    void setIndex(int index) { mIndex = index; }
    int index() const { return mIndex; }

private:
    const char* end() const { return mData + SIZE; }

    char mData[SIZE];
    int mIndex;
    char* mCur;
};

} // namespace tnl

#endif // _FIXED_BUFFER_H_