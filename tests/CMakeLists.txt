cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

set(TEST_SOURCES test_my_thread_pool.cc)

add_executable(
  test_my_thread_pool
  ${TEST_SOURCES}
)
target_link_libraries(
  test_my_thread_pool
  my_thread_pool
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_my_thread_pool)
