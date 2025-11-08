#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "ListReverse.hpp"

TEST(ListReverseTest, BasicReverse)
{
    std::list<int> lst = {1, 2, 3, 4, 5};
    
    ReverseList(lst);
    
    std::vector<int> result(lst.begin(), lst.end());
    std::vector<int> expected = {5, 4, 3, 2, 1};
    
    EXPECT_EQ(result, expected);
}

TEST(ListReverseTest, SingleElement)
{
    std::list<int> lst = {42};
    
    ReverseList(lst);
    
    std::vector<int> result(lst.begin(), lst.end());
    std::vector<int> expected = {42};
    
    EXPECT_EQ(result, expected);
}

TEST(ListReverseTest, EmptyList)
{
    std::list<int> lst;
    
    ReverseList(lst);
    
    EXPECT_TRUE(lst.empty());
}

TEST(ListReverseTest, TwoElements)
{
    std::list<int> lst = {10, 20};
    
    ReverseList(lst);
    
    std::vector<int> result(lst.begin(), lst.end());
    std::vector<int> expected = {20, 10};
    
    EXPECT_EQ(result, expected);
}

TEST(FileIOTest, ReadAndReverseFromFile)
{
    std::ifstream input_file("test_data.txt");
    ASSERT_TRUE(input_file.is_open());
    
    std::ofstream output_file("test_output.txt");
    ASSERT_TRUE(output_file.is_open());
    
    std::string line;
    int test_case = 1;
    
    while (std::getline(input_file, line))
    {
        if (line.empty()) continue;
        
        std::istringstream stream(line);
        std::list<int> lst;
        int num;
        
        while (stream >> num)
        {
            lst.push_back(num);
        }
        
        if (lst.empty()) continue;
        
        output_file << "Test case " << test_case << ":\n";
        PrintList(output_file, lst, "  Original");
        
        ReverseList(lst);
        
        PrintList(output_file, lst, "  Reversed");
        output_file << "\n";
        
        test_case++;
    }
    
    EXPECT_GT(test_case, 1);
}
