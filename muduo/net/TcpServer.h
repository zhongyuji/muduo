#pragma once
#include "muduo/base/noncopyable.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/Callbacks.h"
#include "muduo/net/Acceptor.h"
#include "muduo/net/TcpConnection.h"
#include <map>
#include <string>
#include <atomic>
#include <memory>
class EventLoop;
class TcpServer : noncopyable
{
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }

    void removeConnection(const TcpConnectionPtr& conn);
private:
    void newConnection(int sockfd, const InetAddress& peerAddr);

    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;
    std::string name_;
    std::unique_ptr<Acceptor> acceptor_;

    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    MessageCallback messageCallback_;

    std::atomic<bool> started_;
    int nextConnId_;
    ConnectionMap connections_;
};
