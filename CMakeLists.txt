cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)


include(cmake/FetchGoogleTest.cmake)

project(mylibpp LANGUAGES CXX)

include_directories(include)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)


