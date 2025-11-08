#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "MakeSet.hpp"

TEST(RemoveDuplicatesTest, BasicArray)
{
    std::vector<int> input = {1, 2, 3, 2, 4, 1, 5};
    auto result = RemoveDuplicates(input);
    
    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(result, expected);
}

TEST(RemoveDuplicatesTest, EmptyArray)
{
    std::vector<int> input = {};
    auto result = RemoveDuplicates(input);
    
    EXPECT_TRUE(result.empty());
}

TEST(RemoveDuplicatesTest, NoDuplicates)
{
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = RemoveDuplicates(input);
    
    EXPECT_EQ(result, input);
}

TEST(RemoveDuplicatesTest, AllSame)
{
    std::vector<int> input = {5, 5, 5, 5, 5};
    auto result = RemoveDuplicates(input);
    
    std::vector<int> expected = {5};
    EXPECT_EQ(result, expected);
}

TEST(FileIOTest, ReadProcessWrite)
{
    std::ifstream input_file("testdata.txt");
    ASSERT_TRUE(input_file.is_open()) << "Failed to open testdata.txt";
    
    std::vector<int> numbers;
    bool read_success = ReadNumbers(input_file, numbers);
    
    ASSERT_TRUE(read_success) << "Failed to read numbers from file";
    ASSERT_FALSE(numbers.empty()) << "No numbers were read";
    
    auto unique_numbers = RemoveDuplicates(numbers);
    
    std::ofstream output_file("test_output.txt");
    ASSERT_TRUE(output_file.is_open()) << "Failed to create output file";
    
    PrintNumbers(output_file, numbers, "Original");
    PrintNumbers(output_file, unique_numbers, "Unique");
    
    output_file.close();
    
    EXPECT_LE(unique_numbers.size(), numbers.size());
}

TEST(FileIOTest, MultipleTestCases)
{
    std::ifstream input_file("testdata.txt");
    ASSERT_TRUE(input_file.is_open());
    
    std::ofstream output_file("test_output_multiple.txt");
    ASSERT_TRUE(output_file.is_open());
    
    int test_case = 1;
    std::string line;
    
    while (std::getline(input_file, line))
    {
        if (line.empty()) continue;
        
        std::istringstream stream(line);
        std::vector<int> numbers;
        int num;
        
        while (stream >> num)
        {
            numbers.push_back(num);
        }
        
        if (numbers.empty()) continue;
        
        auto unique_numbers = RemoveDuplicates(numbers);
        
        output_file << "Test case " << test_case << ":\n";
        PrintNumbers(output_file, numbers, "  Input ");
        PrintNumbers(output_file, unique_numbers, "  Output");
        output_file << "\n";
        
        EXPECT_LE(unique_numbers.size(), numbers.size());
        
        test_case++;
    }
    
    EXPECT_GT(test_case, 1) << "No test cases were processed";
}
