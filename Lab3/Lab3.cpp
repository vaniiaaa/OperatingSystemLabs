#include "Runner.hpp"

int main()
{
    int size, nummark;
    std::cout << "Enter array size: ";
    std::cin >> size;
    std::cout << "Enter num of markers: ";
    std::cin >> nummark;
    Runner(size, nummark);
}