cmake_minimum_required(VERSION 3.14)

# Имя для тестового исполняемого файла
set(THIS Lab3Tests)

# Список тестовых файлов
set(SOURCES
    TestsLab3.cpp
)

# Создание исполняемого файла тестов
add_executable(${THIS} ${SOURCES})

# Линковка с библиотеками
target_link_libraries(${THIS}
    PRIVATE
        GTest::gtest_main
        Lab3_lib  # Header-only библиотека с Runner.hpp
)

# Подключение модуля для автообнаружения тестов
include(GoogleTest)

# Автоматическая регистрация тестов
gtest_discover_tests(${THIS})
