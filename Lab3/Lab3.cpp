#include <windows.h>
#include <iostream>
#include <ctime>

CRITICAL_SECTION cs;

struct data
{
    int* Arr;
    int size;
    int markn;
    HANDLE StartEvent, MarkSignal, MainSignal;
    CRITICAL_SECTION* cs;

};

int main()
{
    int size, nummark;
    std::cout << "Enter array size: ";
    std::cin >> size;
    int* Arr = new int[size]();
    std::cout << "Enter num of markers: ";
    std::cin >> nummark;
    HANDLE* MarkSignals = new HANDLE[nummark];
    HANDLE* MainSignals = new HANDLE[nummark];
    HANDLE* Markers = new HANDLE[nummark];
    data* ArrData = new data[nummark];
    HANDLE StartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    for (int i = 0; i < nummark; i++)
    {
        MarkSignals[i] = CreateEvent(
            NULL,     //атрибуты защиты
            TRUE,     //тип события: TRUE - ручной (в несигнальное состояние только ResetEvent), FALSE (так же функциями ожидания)
            FALSE,    //начальное состояние: TRUE - сигнальное
            NULL      //имя ивента для всех потоков
        );
        MainSignals[i] = CreateEvent(
            NULL,     //атрибуты защиты
            TRUE,     //тип события: TRUE - ручной (в несигнальное состояние только ResetEvent), FALSE (так же функциями ожидания)
            FALSE,    //начальное состояние: TRUE - сигнальное
            NULL      //имя ивента для всех потоков
        );
        ArrData[i].Arr = Arr;
        ArrData[i].size = size;
        ArrData[i].markn = i + 1;
        ArrData[i].StartEvent = StartEvent;
        ArrData[i].MainSignal = MainSignals[i];
        ArrData[i].MarkSignal = MarkSignals[i];
        ArrData[i].cs = &cs;
        
        Markers[i] = CreateThread(
            NULL,
            0,
            marker,
            &ArrData[i],
            0,
            NULL
        )
    }
    


    InitializeCriticalSection(&cs);


    DeleteCriticalSection(&cs);
    delete Arr;
}