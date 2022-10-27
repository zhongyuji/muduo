set(net_SRCS 
    EventLoop.cc
    Channel.cc
    Poller.cc
    Timer.cc
    TimerQueue.cc
    SocketsOps.cc
)

add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net muduo_base)
