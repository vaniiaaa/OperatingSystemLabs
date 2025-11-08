#ifndef MAKESET_HPP
#define MAKESET_HPP

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <limits>


template <typename T>
std::vector<T> RemoveDuplicates(const std::vector<T>& container) 
{
    std::unordered_set<T> seen;
    std::vector<T> result;
    result.reserve(container.size());
    
    for (const auto& element : container) 
    {
        if (seen.find(element) == seen.end()) 
        {
            seen.insert(element);
            result.push_back(element);
        }
    }
    
    return result;
}


bool ReadNumbers(std::istream& input, std::vector<int>& numbers) 
{
    std::string line;
    
    if (!std::getline(input, line)) 
    {
        return false;
    }
    
    if (line.empty()) 
    {
        return false;
    }
    
    std::istringstream stream(line);
    int number;
    
    while (stream >> number) 
    {
        numbers.push_back(number);
    }
    
    if (stream.fail() && !stream.eof()) 
    {
        return false;
    }
    
    if (numbers.empty()) 
    {
        return false;
    }
    
    return true;
}

void PrintNumbers(std::ostream& output, const std::vector<int>& numbers, const std::string& label) 
{
    output << label << ": ";
    for (size_t i = 0; i < numbers.size(); ++i) 
    {
        output << numbers[i];
        if (i < numbers.size() - 1) 
        {
            output << " ";
        }
    }
    output << "\n";
}

int Run() 
{
    try 
    {
        std::vector<int> numbers;
        
        std::cout << "Enter numbers separated by spaces: ";
        
        if (!ReadNumbers(std::cin, numbers)) 
        {
            std::cerr << "Error: Invalid input\n";
            return 1;
        }
        
        PrintNumbers(std::cout, numbers, "Original array");
        
        auto unique_numbers = RemoveDuplicates(numbers);
        
        PrintNumbers(std::cout, unique_numbers, "Array without duplicates");
        
        return 0;
        
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 2;
    }
}

#endif // MAKESET_HPP
