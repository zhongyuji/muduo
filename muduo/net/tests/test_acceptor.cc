#include "muduo/net/EventLoop.h"
#include "muduo/net/Acceptor.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/SocketsOps.h"

void newConnection(int sockfd, const InetAddress& addr)
{
    printf("newConnection accepted a new connect from \n");
    ::write(sockfd, "How are you?\n", 13);
    sockets::close(sockfd);
}
int main(int args, char** argv)
{
    printf("main() pid = %d", getpid());
    InetAddress addr(9981);
    EventLoop loop;
    Acceptor acceptor(&loop, addr, true);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();
    return 0;
}
