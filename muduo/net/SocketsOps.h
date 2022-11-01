#include <aio.h>
#include <fcntl.h>

namespace sockets {
    ssize_t read(int sockfd, void *buf, size_t count);
    ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
    ssize_t write(int sockfd, const void *buf, size_t count);
} //namespace sockets
