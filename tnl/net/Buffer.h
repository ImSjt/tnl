#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <assert.h>
#include <string.h>
#include <algorithm>
#include <vector>

#include "tnl/base/copyable.h"
#include "tnl/net/SocketsOps.h"

namespace tnl
{
namespace net
{

class Buffer : copyable
{
public:
    explicit Buffer(size_t initialSize = mInitialSize) :
        mBuffer(mCheapPrepend + initialSize),
        mReaderIndex(mCheapPrepend),
        mWriterIndex(mCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == mCheapPrepend);
    }

    void swap(Buffer& rhs)
    {
        mBuffer.swap(rhs.mBuffer);
        std::swap(mReaderIndex, rhs.mReaderIndex);
        std::swap(mWriterIndex, rhs.mWriterIndex);
    }

    size_t readableBytes() const
    {
        return mWriterIndex - mReaderIndex;
    }

    size_t writableBytes() const
    {
        return mBuffer.size() - mWriterIndex;
    }

    size_t prependableBytes() const
    {
        return mReaderIndex;
    }

    const char* peek() const
    { return begin() + mReaderIndex; }

    const char* findCRLF() const
    {
        const char* crlf = std::search(peek(), beginWrite(), mCRLF, mCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char* start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
    
        const char* crlf = std::search(start, beginWrite(), mCRLF, mCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findEOL() const
    {
        const void* eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char*>(eol);
    }

    const char* findEOL(const char* start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const void* eol = memchr(start, '\n', beginWrite() - start);
        return static_cast<const char*>(eol);
    }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes())
        {
            mReaderIndex += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveInt64()
    {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32()
    {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16()
    {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8()
    {
        retrieve(sizeof(int8_t));
    }

    void retrieveAll()
    {
        mReaderIndex = mCheapPrepend;
        mWriterIndex = mCheapPrepend;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void append(const char* /*restrict*/ data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void append(const void* /*restrict*/ data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char* beginWrite()
    { return begin() + mWriterIndex; }

    const char* beginWrite() const
    { return begin() + mWriterIndex; }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        mWriterIndex += len;
    }

    void unwrite(size_t len)
    {
        assert(len <= readableBytes());
        mWriterIndex -= len;
    }

    void appendInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        append(&be64, sizeof be64);
    }

    void appendInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        append(&be32, sizeof be32);
    }

    void appendInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        append(&be16, sizeof be16);
    }

    void appendInt8(int8_t x)
    {
        append(&x, sizeof x);
    }

    int64_t readInt64()
    {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }

    int32_t readInt32()
    {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }

    int16_t readInt16()
    {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }

    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }

    int64_t peekInt64() const
    {
        assert(readableBytes() >= sizeof(int64_t));
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return sockets::networkToHost64(be64);
    }

    int32_t peekInt32() const
    {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return sockets::networkToHost32(be32);
    }

    int16_t peekInt16() const
    {
        assert(readableBytes() >= sizeof(int16_t));
        int16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof be16);
        return sockets::networkToHost16(be16);
    }

    int8_t peekInt8() const
    {
        assert(readableBytes() >= sizeof(int8_t));
        int8_t x = *peek();
        return x;
    }

    void prependInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        prepend(&be64, sizeof be64);
    }

    void prependInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        prepend(&be32, sizeof be32);
    }

    void prependInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        prepend(&be16, sizeof be16);
    }

    void prependInt8(int8_t x)
    {
        prepend(&x, sizeof x);
    }

    void prepend(const void* /*restrict*/ data, size_t len)
    {
        assert(len <= prependableBytes());
        mReaderIndex -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, begin()+mReaderIndex);
    }

    void shrink(size_t reserve)
    {
        Buffer other;
        other.ensureWritableBytes(readableBytes()+reserve);
        other.append(peek(), readableBytes());
        swap(other);
    }

    size_t internalCapacity() const
    {
        return mBuffer.capacity();
    }

    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin()
    { return &*mBuffer.begin(); }

    const char* begin() const
    { return &*mBuffer.begin(); }

    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + mCheapPrepend)
        {
            mBuffer.resize(mWriterIndex+len);
        }
        else
        {
            assert(mCheapPrepend < mReaderIndex);
            size_t readable = readableBytes();
            std::copy(begin()+mReaderIndex,
                        begin()+mWriterIndex,
                        begin()+mCheapPrepend);
            mReaderIndex = mCheapPrepend;
            mWriterIndex = mReaderIndex + readable;
            assert(readable == readableBytes());
        }
    }

private:
    std::vector<char> mBuffer;
    size_t mReaderIndex;
    size_t mWriterIndex;

    static const char mCRLF[];
    static const size_t mCheapPrepend = 8; // 在首部预留一段空间
    static const size_t mInitialSize = 1024;
};

}
}

#endif // _BUFFER_H_