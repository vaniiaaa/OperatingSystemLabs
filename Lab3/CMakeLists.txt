cmake_minimum_required(VERSION 3.14)

set(THIS Lab3)

project(${THIS} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Список заголовочных файлов
set(HEADERS
    MultiThreads.hpp
    Runner.hpp
)

# Создание INTERFACE библиотеки для header-only кода
add_library(${THIS}_lib INTERFACE)
target_sources(${THIS}_lib INTERFACE ${HEADERS})
target_include_directories(${THIS}_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Основная программа
add_executable(${THIS} Lab3.cpp)
target_link_libraries(${THIS} PRIVATE ${THIS}_lib)

# Подключение Google Test через FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Включение тестирования
enable_testing()

# Подключение директории с тестами
add_subdirectory(tests)
