#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <memory>
#include <string>

#include "tnl/base/noncopyable.h"
#include "tnl/net/Buffer.h"
#include "tnl/net/EventLoop.h"
#include "tnl/net/InetAddress.h"
#include "tnl/net/Socket.h"

namespace tnl
{
namespace net
{

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                    const InetAddress& localAddr, const InetAddress& peerAddr);
    virtual ~TcpConnection();

    EventLoop* getLoop() const { return mLoop; }
    const std::string& name() const { return mName; }
    const InetAddress& localAddress() const { return mLocalAddr; }
    const InetAddress& peerAddress() const { return mPeerAddr; }
    bool connected() const { return mState == Connected; }
    bool disconnected() const { return mState == Disconnected; }

    void send(const char* message, int len);
    void send(const std::string& message);
    void send(Buffer* message);

    void shutdown(); 

    void forceClose();
    void setTcpNoDelay(bool on);
    
    void startRead();
    void stopRead();
    bool isReading() const { return mReading; };

    void setHighWaterMark(size_t highWaterMark)
    { mHighWaterMark = highWaterMark; }

    Buffer* inputBuffer()
    { return &mInputBuffer; }

    Buffer* outputBuffer()
    { return &mOutputBuffer; }

    // 设置关闭回调函数
    void setCloseCallback(const CloseCallback& cb)
    { mCloseCallback = cb; }

    void connectEstablished();

    void connectDestroyed();

protected:
    // 提供接口
    virtual void handleConnection();    // 连接建立或者连接断开
    virtual void handleBytes(Buffer*);  // 处理请求
    virtual void handleWriteComplete(); // 写完成
    virtual void highWaterMark(size_t); // 超过警戒线时的写处理

private:
    enum ConnState { Disconnected, Connecting, Connected, Disconnecting };

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();
    
    void sendInLoop(const char* message, size_t len);
    void sendInLoop(std::string& message);

    void shutdownInLoop();
    void forceCloseInLoop();
    void setState(ConnState s) { mState = s; }
    const char* stateToString() const;
    void startReadInLoop();
    void stopReadInLoop();

    void writeCompleteCallback();
    void highWaterMarkCallback(size_t);

private:
    EventLoop* mLoop;
    const std::string mName;
    ConnState mState;
    bool mReading;

    std::unique_ptr<Socket> mSocket;
    std::unique_ptr<Channel> mChannel;
    const InetAddress mLocalAddr;
    const InetAddress mPeerAddr;
    // ConnectionCallback mConnectionCallback;
    // MessageCallback mMessageCallback;
    // WriteCompleteCallback mWriteCompleteCallback;
    // HighWaterMarkCallback mHighWaterMarkCallback;
    CloseCallback mCloseCallback;
    size_t mHighWaterMark;
    Buffer mInputBuffer;
    Buffer mOutputBuffer;
};

} // namespace net
} // namespace tnl

#endif // _TCP_CONNECTION_H_