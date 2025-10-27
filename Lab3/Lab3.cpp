#include <windows.h>
#include <iostream>
#include <ctime>

CRITICAL_SECTION cs;

int main()
{
    int size, nummark;
    std::cout << "Enter array size: ";
    std::cin >> size;
    int* arr = new int[size];
    std::cout << "Enter num of markers: ";
    std::cin >> nummark;
    InitializeCriticalSection(&cs);


    DeleteCriticalSection(&cs);
    delete arr;
}