cmake_minimum_required(VERSION 3.22)
project(io)

add_subdirectory(context)

add_executable(mkdir examples/mkdir.cpp)
add_executable(defer examples/defer.cpp)
add_executable(ctx examples/ctx.cpp)

target_link_libraries(mkdir PUBLIC uring)
target_link_libraries(defer PUBLIC uring)
target_link_libraries(ctx PUBLIC context)