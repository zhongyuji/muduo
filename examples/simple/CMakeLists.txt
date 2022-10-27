add_executable(simpleEventLoop simpleEventLoop.cc)
target_link_libraries(simpleEventLoop muduo_net)

add_executable(test_RectorEvent test_RectorEvent.cc)
target_link_libraries(test_RectorEvent muduo_net)

add_executable(test_runAfter test_runAt.cc)
target_link_libraries(test_runAfter muduo_net)

