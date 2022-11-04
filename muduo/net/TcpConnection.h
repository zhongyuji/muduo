#pragma once
#include "muduo/base/noncopyable.h"
#include "muduo/net/Callbacks.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/Channel.h"
#include "muduo/net/Socket.h"
#include <memory>
#include <string>
#include <functional>

class EventLoop;
class InetAddress;
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }
    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

    const std::string& name() const { return name_; }
    bool connected() const { return state_ == kConnected; }
    Buffer* inputBuffer() { return &inputBuffer_; }
    Buffer* outputBuffer() { return &outputBuffer_; }

    void connectEstablished();
    const char* stateToString() const;

    void connectDestoryed();
private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting};

    void setState(StateE s) { state_ = s; }
    void handleRead();
    void handleWirte();
    void handleClose();
    void handleError();

    EventLoop* loop_;
    const std::string name_;
    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    const InetAddress localAddr_;
    const InetAddress& peerAddr_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    MessageCallback messageCallback_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};
