cmake_minimum_required(VERSION 3.10)
project(Lab2 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE 0)
endif()

add_executable(lab2 Lab2.cpp)


if(WIN32)
    target_link_libraries(lab2 Kernel32.lib)
endif()

