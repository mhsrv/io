cmake_minimum_required(VERSION 3.22)
add_library(async STATIC lib/event-loop.h lib/async.h lib/io.h src/event-loop.cpp lib/async.h src/async.cpp src/io.cpp)
target_include_directories(async PUBLIC lib)
target_link_libraries(async uring co utils)
target_compile_definitions(async PUBLIC IO_EVENT_LOOP_HIGH_EFFICIENCY)