#include "muduo/net/SocketsOps.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
    return ::read(sockfd, buf, count);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
    return ::write(sockfd, buf, count);
}


