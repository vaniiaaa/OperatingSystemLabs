#ifndef LIST_REVERSE_HPP
#define LIST_REVERSE_HPP

#include <iostream>
#include <list>
#include <forward_list>
#include <vector>
#include <sstream>

template <typename T>
void ReverseRecursive(std::list<T>& lst, typename std::list<T>::iterator current)
{
    if (current == lst.end())
    {
        return;
    }
    
    auto next_it = std::next(current);
    
    ReverseRecursive(lst, next_it);
    
    if (next_it != lst.end())
    {
        lst.splice(lst.end(), lst, current);
    }
}

template <typename T>
void ReverseList(std::list<T>& lst)
{
    if (!lst.empty())
    {
        ReverseRecursive(lst, lst.begin());
    }
}

template <typename T>
void PrintList(std::ostream& output, const std::list<T>& lst, const std::string& label)
{
    output << label << ": ";
    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        output << *it;
        if (std::next(it) != lst.end())
        {
            output << " -> ";
        }
    }
    output << "\n";
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
    
    if (numbers.empty())
    {
        return false;
    }
    
    return true;
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
        
        std::list<int> lst(numbers.begin(), numbers.end());
        
        PrintList(std::cout, lst, "Original list");
        
        ReverseList(lst);
        
        PrintList(std::cout, lst, "Reversed list");
        
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 2;
    }
}

#endif
