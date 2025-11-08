#include <gtest/gtest.h>
#include "Fact.hpp"


TEST(FindFactorialsTest, SmallNumbers) {
    auto result = FindFactorials(5);
    
    EXPECT_EQ(result.size(), 6);
    EXPECT_EQ(result[0], 1);      
    EXPECT_EQ(result[1], 1);      
    EXPECT_EQ(result[2], 2);      
    EXPECT_EQ(result[3], 6);      
    EXPECT_EQ(result[4], 24);     
    EXPECT_EQ(result[5], 120);    
}

TEST(FindFactorialsTest, FactorialZero) 
{
    auto result = FindFactorials(0);
    
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 1);
}

TEST(FindFactorialsTest, ModuloArithmetic) 
{
    auto result = FindFactorials(100);
    
    EXPECT_EQ(result.size(), 101);
    
    for (const auto& val : result) 
    {
        EXPECT_LT(val, 1000000007);
        EXPECT_GE(val, 0);
    }
}

TEST(FindFactorialsTest, FactorialSequence) 
{
    auto result = FindFactorials(10);
    
    EXPECT_EQ(result.size(), 11);
    EXPECT_EQ(result[10], 3628800); 

    for (int i = 1; i <= 10; ++i) 
    {
        long long expected = (static_cast<long long>(result[i - 1]) * i) % MOD;
        EXPECT_EQ(result[i], static_cast<int>(expected));
    }

    std::system("pause");
}
