cmake_minimum_required(VERSION 3.22)
project(io)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("-- Building in debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O0 -ggdb3 -fsized-deallocation")
else()
    message("-- Building in release mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Ofast -fsized-deallocation")
endif()

add_subdirectory(deps)
add_subdirectory(async)
add_subdirectory(example)
