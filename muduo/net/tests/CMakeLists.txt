#if(BOOSTTEST_LIBRARY)
add_executable(buffer_unittest Buffer_unittest.cc)
target_link_libraries(buffer_unittest muduo_net boost_unit_test_framework)
add_test(NAME buffer_unittest COMMAND buffer_unittest)
#endif()

add_executable(test_acceptor test_acceptor.cc)
target_link_libraries(test_acceptor muduo_net)
