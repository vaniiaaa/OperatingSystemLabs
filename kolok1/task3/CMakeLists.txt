cmake_minimum_required(VERSION 3.14)

set(THIS LinkedListProject)

project(${THIS} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(HEADERS
    ListReverse.hpp
)

add_library(${THIS}_lib INTERFACE)
target_sources(${THIS}_lib INTERFACE ${HEADERS})
target_include_directories(${THIS}_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${THIS} Main.cpp)
target_link_libraries(${THIS} PRIVATE ${THIS}_lib)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(Test)
