#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/Acceptor.h"
#include "muduo/net/SocketsOps.h"
#include "muduo/net/EventLoopThreadPool.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg)
    :   loop_(loop),
        name_(nameArg),
        acceptor_(new Acceptor(loop, listenAddr, true)),
        threadPool_(new EventLoopThreadPool(loop, "ThreadPool")),
        connectionCallback_(defaultConnectionCallback),
        messageCallback_(defaultMessageCallback),
        started_(false),
        nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    loop_->assertInLoopThread();
    printf("TcpServer::~TcpServer [%s] destructing\n", name_.c_str());
    for (auto& item : connections_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        //conn->getLoop()->runInLoop(std::bind(&TcpServer::connectDestroyed, conn));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    EventLoop* ioLoop = threadPool_->getNextLoop();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    printf("TcpServer::newConnection name = %s, new connection name = %s, form %s.\n", name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                          connName,
                                          sockfd,
                                          localAddr,
                                          peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::start()
{
   if (started_ == false) {
        started_ = true;
        assert(!acceptor_->listening());
        loop_->runInLoop(std::bind(&Acceptor::listen, get_pointer(acceptor_)));
   } 
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    printf("TcpServer::removeConnection [%s] connection name = %s\n", name_.c_str(), conn->name().c_str());
    size_t n = connections_.erase(conn->name());
    assert(n == 1); (void)n;
    loop_->queueInLoop(std::bind(&TcpConnection::connectDestoryed, conn));
}
