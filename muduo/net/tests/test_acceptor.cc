#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/SocketsOps.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/TcpConnection.h"

void newConnection(const TcpConnectionPtr& conn)
{
    printf("newConnection accepted a new connect from %s\n", conn->name().c_str());
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timerstamp tm)
{
    std::string msg(buf->retrieveAllAsString());
    printf("%s\n", msg.c_str());  
}

int main(int args, char** argv)
{
    printf("main() pid = %d", getpid());
    InetAddress addr(9981);
    EventLoop loop;
    TcpServer server(&loop, addr, "TcpServer");
    server.setConnectionCallback(newConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
    return 0;
}
