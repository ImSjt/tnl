#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <memory>
#include <map>
#include <functional>
#include <atomic>

#include "tnl/base/noncopyable.h"
#include "tnl/net/Acceptor.h"
#include "tnl/net/TcpConnection.h"
#include "tnl/net/EventLoopThreadPool.h"

namespace tnl
{
namespace net
{

class TcpServer : noncopyable
{
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr,
                const std::string& name, bool reusePort = false);
    virtual ~TcpServer();

    const std::string& ipPort() const { return mIpPort; }
    const std::string& name() const { return mName; }
    EventLoop* getLoop() const { return mLoop; }

    void setThreadNum(int numThreads);

    std::shared_ptr<EventLoopThreadPool> threadPool()
    { return mThreadPools; }

    void start();

protected:
    virtual TcpConnection* createNewConnection(EventLoop* loop,
                    const std::string& name, int sockfd,
                    const InetAddress& localAddr, const InetAddress& peerAddr);

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

private:
    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    EventLoop* mLoop;
    const std::string mIpPort;
    const std::string mName;
    std::atomic<int> mStarted;

    std::unique_ptr<Acceptor> mAcceptor;
    std::shared_ptr<EventLoopThreadPool> mThreadPools;

    // 总是在 baseloop 中调用
    int mNextConnId;
    ConnectionMap mConnections;

};

}
}

#endif // _TCP_SERVER_H_