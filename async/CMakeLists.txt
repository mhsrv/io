cmake_minimum_required(VERSION 3.22)
add_library(async STATIC lib/work.h src/work.cpp
        lib/channel.h
        lib/scheduler.h
        src/scheduler.cpp
        lib/event-loop.h
        src/event-loop.cpp
        lib/async.h
        src/async.cpp
        lib/io.h
        src/io.cpp
)
target_include_directories(async PUBLIC lib)
target_link_libraries(async uring co utils MPMCQueue)
target_compile_definitions(async PUBLIC IO_EVENT_LOOP_USE_WORKERS IO_EVENT_LOOP_HIGH_EFFICIENCY)