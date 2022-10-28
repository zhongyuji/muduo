set(base_SRCS
    CountDownLatch.cc
    Timerstamp.cc    
    CurrentThread.cc
    Thread.cc
    )

add_library(muduo_base ${base_SRCS})

add_subdirectory(tests)
