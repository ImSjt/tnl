#include <errno.h>
#include <functional>

#include "tnl/base/Logger.h"
#include "tnl/net/SocketsOps.h"
#include "tnl/net/TcpConnection.h"

using namespace tnl;
using namespace tnl::net;

static void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    LOG_TRACE("%s -> %s is %s", conn->localAddress().toIpPort().c_str(),
                                conn->peerAddress().toIpPort().c_str(),
                                (conn->connected() ? "UP" : "DOWN"));
}

static void defaultMessageCallback(const TcpConnectionPtr&,
                                        Buffer* buf)
{
    LOG_TRACE("get msg bytes(%d)", buf->readableBytes());
    buf->retrieveAll();
}

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                const InetAddress& localAddr, const InetAddress& peerAddr) :
    mLoop(loop),
    mName(name),
    mState(Connecting),
    mReading(true),
    mSocket(new Socket(sockfd)),
    mChannel(new Channel(loop, sockfd)),
    mLocalAddr(localAddr),
    mPeerAddr(peerAddr),
    mHighWaterMark(64*1024*1024), // 发送缓存警告线
    mConnectionCallback(defaultConnectionCallback),
    mMessageCallback(defaultMessageCallback)
{
    mChannel->setReadCallback(
        std::bind(&TcpConnection::handleRead, this));
    mChannel->setWriteCallback(
        std::bind(&TcpConnection::handleWrite, this));
    mChannel->setCloseCallback(
        std::bind(&TcpConnection::handleClose, this));
    mChannel->setErrorCallback(
        std::bind(&TcpConnection::handleError, this));

    mSocket->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_TRACE("TcpConnection::dtor[%s] at %p fd=%d state=%s",
                mName.c_str(), this, mSocket->fd(), stateToString());
    assert(mState == Disconnected);
}

void TcpConnection::send(const char* data, int len)
{
    if (mState == Connected)
    {
        if (mLoop->isInLoopThread())
        {
            sendInLoop(data, len);
        }
        else
        {
            // 将数据复制一份，放到对应的loop中运行，使用右值引用来减少拷贝
            void (TcpConnection::*fp)(std::string&) = &TcpConnection::sendInLoop;
            mLoop->runInLoop(std::bind(fp,
                                        this,
                                        std::string(data, len)));
        }
    }
}

void TcpConnection::send(const std::string& message)
{
    send(message.c_str(), message.size());
}

void TcpConnection::send(Buffer* buf)
{
    if (mState == Connected)
    {
        send(buf->peek(), buf->readableBytes());
        buf->retrieveAll();
    }
}

void TcpConnection::shutdown()
{
    if (mState == Connected)
    {
        setState(Disconnected);
        mLoop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::forceClose()
{
    if (mState == Connected || mState == Disconnecting)
    {
        setState(Disconnecting);
        mLoop->queueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::setTcpNoDelay(bool on)
{
    mSocket->setTcpNoDelay(on);
}

void TcpConnection::startRead()
{
    mLoop->runInLoop(std::bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::stopRead()
{
    mLoop->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::connectEstablished()
{
    assert(mLoop->isInLoopThread());
    assert(mState == Connecting);
    setState(Connected);
    
    mChannel->enableReading();

    mConnectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    assert(mLoop->isInLoopThread());
    LOG_TRACE("connect destroy,state=%s", stateToString());
    if (mState == Connected)
    {
        setState(Disconnected);
        mChannel->disableAll();

        mConnectionCallback(shared_from_this());
    }
    
    mChannel->remove();
}

void TcpConnection::handleRead()
{
    assert(mLoop->isInLoopThread());
    int savedErrno = 0;
    ssize_t n = mInputBuffer.readFd(mChannel->fd(), &savedErrno);
    if (n > 0)
    {
        mMessageCallback(shared_from_this(), &mInputBuffer);
    }
    else if (n == 0)
    {
        LOG_TRACE("client close");
        handleClose();
    }
    else
    {
        errno = savedErrno;
        LOG_ERROR("TcpConnection::handleRead");
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    assert(mLoop->isInLoopThread());
    if (mChannel->isWriting())
    {
        ssize_t n = sockets::write(mChannel->fd(),
                                mOutputBuffer.peek(),
                                mOutputBuffer.readableBytes());
        if (n > 0)
        {
            mOutputBuffer.retrieve(n);
            if (mOutputBuffer.readableBytes() == 0)
            {
                mChannel->disableWriting();
                if (mWriteCompleteCallback)
                {
                    mLoop->queueInLoop(std::bind(mWriteCompleteCallback, shared_from_this()));
                }
                if (mState == Disconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite");
        }
    }
    else
    {
        LOG_TRACE("Connection fd=%d is down, no more writing", mChannel->fd());
    }
}

void TcpConnection::handleClose()
{
    assert(mLoop->isInLoopThread());

    LOG_TRACE("fd=%d state=%s", mChannel->fd(), stateToString());
    assert(mState == Connected || mState == Disconnecting);

    setState(Disconnected);
    mChannel->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    mConnectionCallback(guardThis);
    
    mCloseCallback(guardThis);
}

void TcpConnection::handleError()
{
    int err = sockets::getSocketError(mChannel->fd());
    LOG_ERROR("TcpConnection::handleError [%s] - SO_ERROR=%d", mName.c_str(), err);
}

void TcpConnection::sendInLoop(const char* message, size_t len)
{
    assert(mLoop->isInLoopThread());
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    // 如果现在连接状态，那么就直接退出
    if (mState == Disconnected)
    {
        LOG_WARN("disconnected, give up writing");
        return;
    }

    if (!mChannel->isWriting() && mOutputBuffer.readableBytes() == 0) // 如果通道不可写并且缓存区没有数据
    {
        // 直接发送
        nwrote = sockets::write(mChannel->fd(), message, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;

            // 发送完后调用写完成回调
            if (remaining == 0 && mWriteCompleteCallback)
            {
                mLoop->queueInLoop(std::bind(mWriteCompleteCallback, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                LOG_ERROR("TcpConnection::sendInLoop");
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    assert(remaining <= len);

    // 如果写未完成
    if (!faultError && remaining > 0)
    {
        size_t oldLen = mOutputBuffer.readableBytes(); // 缓存区中剩余的数据
        if (oldLen + remaining >= mHighWaterMark       // 如果总的数据高于警告线
                    && oldLen < mHighWaterMark
                    && mHighWaterMarkCallback)
        {
            // 那么就使用 mHighWaterMarkCallback
            mLoop->queueInLoop(std::bind(mHighWaterMarkCallback, shared_from_this(), oldLen + remaining));
        }

        mOutputBuffer.append(message+nwrote, remaining);
        if (!mChannel->isWriting())
        {
            mChannel->enableWriting();
        }
    }
}

void TcpConnection::sendInLoop(std::string& message)
{
    sendInLoop(message.c_str(), message.size());
}

void TcpConnection::shutdownInLoop()
{
    assert(mLoop->isInLoopThread());

    if (!mChannel->isWriting())
    {
        mSocket->shutdownWrite();
    }
}

void TcpConnection::forceCloseInLoop()
{
    assert(mLoop->isInLoopThread());
    if (mState == Connected || mState == Disconnecting)
    {
        handleClose();
    }
}

const char* TcpConnection::stateToString() const
{
    switch (mState)
    {
        case Disconnected:
            return "Disconnected";
        case Connecting:
            return "Connecting";
        case Connected:
            return "Connected";
        case Disconnecting:
            return "Disconnecting";
        default:
            return "unknown state";
    }
}

void TcpConnection::startReadInLoop()
{
    assert(mLoop->isInLoopThread());
    
    if (!mReading || !mChannel->isReading())
    {
        mChannel->enableReading();
        mReading = true;
    }
}

void TcpConnection::stopReadInLoop()
{
    assert(mLoop->isInLoopThread());
    if (mReading || mChannel->isReading())
    {
        mChannel->disableReading();
        mReading = false;
    }
}