cmake_minimum_required(VERSION 3.22)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/)

include(FetchContent)
FetchContent_Declare(
        co
        GIT_REPOSITORY https://github.com/mihail-8480/libco-amd64-cmake
        GIT_TAG master
)
FetchContent_MakeAvailable(co)

