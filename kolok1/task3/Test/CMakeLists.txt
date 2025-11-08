add_executable(${THIS}Tests test.cpp)

target_link_libraries(${THIS}Tests 
    PRIVATE 
    ${THIS}_lib
    GTest::gtest 
    GTest::gtest_main
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_data.txt
    ${CMAKE_BINARY_DIR}/test_data.txt
    COPYONLY
)

include(GoogleTest)
gtest_discover_tests(${THIS}Tests 
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
