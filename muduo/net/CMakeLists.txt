set(net_SRCS 
    Acceptor.cc
    Buffer.cc
    EventLoop.cc
    EventLoopThread.cc
    Channel.cc
    Poller.cc
    TcpServer.cc
    TcpConnection.cc
    Timer.cc
    TimerQueue.cc
    Socket.cc
    SocketsOps.cc
    InetAddress.cc
)

add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net muduo_base)

add_subdirectory(tests)
