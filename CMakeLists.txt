cmake_minimum_required(VERSION 3.22)
project(io)

set(CMAKE_CXX_STANDARD 23)

add_executable(mkdir examples/mkdir.cpp)
target_link_libraries(mkdir PUBLIC uring)