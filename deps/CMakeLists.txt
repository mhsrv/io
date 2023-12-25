cmake_minimum_required(VERSION 3.22)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/)

include(FetchContent)
FetchContent_Declare(
        co
        GIT_REPOSITORY https://github.com/mhsrv/libco-cmake
        GIT_TAG master
)
FetchContent_MakeAvailable(co)

FetchContent_Declare(
        utils
        GIT_REPOSITORY https://github.com/mhsrv/utils
        GIT_TAG master
)
FetchContent_MakeAvailable(utils)

FetchContent_Declare(
        MPMCQueue
        GIT_REPOSITORY https://github.com/rigtorp/MPMCQueue
        GIT_TAG master
)
FetchContent_MakeAvailable(MPMCQueue)