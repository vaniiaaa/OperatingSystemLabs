# Создаем исполняемый файл для тестов
add_executable(${THIS}Tests test.cpp)

# Линкуем библиотеку проекта и Google Test
target_link_libraries(${THIS}Tests 
    PRIVATE 
    ${THIS}_lib
    GTest::gtest 
    GTest::gtest_main
)

# Регистрируем тесты в CTest
include(GoogleTest)
gtest_discover_tests(${THIS}Tests)
