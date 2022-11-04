#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpConnection.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/SocketsOps.h"
void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
  printf("%s -> %s is %d\n",conn->localAddress().toIpPort().c_str(), conn->peerAddress().toIpPort().c_str(), conn->connected());
  // do not call conn->forceClose(), because some users want to register message callback only.
}

void defaultMessageCallback(const TcpConnectionPtr&,
                                        Buffer* buf,
                                        Timerstamp)
{
    //buf->retrieveAll();
    printf("%s\n", buf->retrieveAllAsString().c_str());
}

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr) 
    :   loop_(loop),
        name_(name),
        state_(kConnecting),
        socket_(new Socket(sockfd)),
        channel_(new Channel(loop, sockfd)),
        localAddr_(localAddr),
        peerAddr_(peerAddr)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
}

TcpConnection::~TcpConnection()
{
    printf("TcpConnector::dtor[%s] fd = %d, state = %s", name_.c_str(), channel_->fd(), stateToString());
}

void TcpConnection::handleRead()
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, Timerstamp(Timerstamp::now()));
    }
    else if (n == 0) {
        handleClose();
    }
    else {
        errno = savedErrno;
        printf("TcpConnection::handleRead error\n");
        handleError();
    }
}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();
}

const char* TcpConnection::stateToString() const
{
  switch (state_)
  {
    //case kDisconnected:
      //return "kDisconnected";
    case kConnecting:
      return "kConnecting";
    case kConnected:
      return "kConnected";
    //case kDisconnecting:
      //return "kDisconnecting";
    default:
      return "unknown state";
  }
}

void TcpConnection::handleWirte()
{
    
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    printf("TcpConnection::handleClose state = %d", state_);
    assert(state_ == kConnected);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    int err = sockets::getSocketError(channel_->fd());
    printf("TcpConnection::handleError [%s] - SO_ERROR = %d", name_.c_str(), err);
}

void TcpConnection::connectDestoryed()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    channel_->enableReading();

    loop_->removeChannel(get_pointer(channel_));
}

