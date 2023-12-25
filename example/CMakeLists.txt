cmake_minimum_required(VERSION 3.22)
project(io)
add_executable(test test.cpp)
target_link_libraries(test async)