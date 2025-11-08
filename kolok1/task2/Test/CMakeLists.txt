add_executable(${THIS}Tests test.cpp)

target_link_libraries(${THIS}Tests 
    PRIVATE 
    ${THIS}_lib
    GTest::gtest 
    GTest::gtest_main
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/testdata.txt
    ${CMAKE_BINARY_DIR}/testdata.txt
    COPYONLY
)

include(GoogleTest)
gtest_discover_tests(${THIS}Tests)
