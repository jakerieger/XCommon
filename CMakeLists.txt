cmake_minimum_required(VERSION 3.14)
project(XCommon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.9.1
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(Catch2)

# Include Catch2's CMake modules for test discovery
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

include_directories(${CMAKE_SOURCE_DIR}/Code)

# Include tests
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/Tests)

include(${TESTS_DIR}/DateTime/Test.DateTime.cmake)