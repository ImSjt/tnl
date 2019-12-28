#include <errno.h>
#include <sys/uio.h>

#include "tnl/net/Buffer.h"

using namespace tnl;
using namespace tnl::net;

const char Buffer::mCRLF[] = "\r\n";

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin()+mWriterIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        mWriterIndex += n;
    }
    else
    {
        mWriterIndex = mBuffer.size();
        append(extrabuf, n - writable);
    }

    return n;
}

