#pragma once 
#include "muduo/base/noncopyable.h"
#include "muduo/net/Channel.h"
#include "muduo/net/Socket.h"
#include <functional>

class EventLoop;
class Acceptor : public noncopyable
{
public:
    typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb;}

    void listen();

    bool listening() const { return listening_; }
private:
    void handleRead();

    EventLoop* loop_;
    bool listening_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    int idleFd_;
};
