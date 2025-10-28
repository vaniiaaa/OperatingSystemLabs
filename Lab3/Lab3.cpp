#include <windows.h>
#include <iostream>
#include <ctime>


CRITICAL_SECTION cs;

struct Data
{
    int* Arr;
    int size;
    int markn;
    HANDLE StartEvent, MarkSignal, MainSignal;
    CRITICAL_SECTION* cs;

};

DWORD WINAPI marker(LPVOID _data)
{
    Data* data = (Data*)_data;
    std::srand(8);
    int count = 0 ;
    WaitForSingleObject(data->MainSignal, INFINITE);
    bool cont = true;
    while(cont) {
        int index = rand() % data->size;
        EnterCriticalSection(data->cs);
        if (data->Arr[index] == 0)
        {
            Sleep(5);
            data->Arr[index] = data->markn;
            Sleep(5);
            count++;
            LeaveCriticalSection(data->cs);
        }
        else 
        {
            LeaveCriticalSection(data->cs);
            std::cout << "Marker num: " << data->markn << " Num of changed pos: " << count << " Index of arr: " << index << '\n';
            SetEvent(data->MarkSignal);
            ResetEvent(data->MainSignal);
            if (WaitForSingleObject(data->MainSignal, INFINITE) == WAIT_OBJECT_0) continue;
            else 
            {
                cont = false;
                EnterCriticalSection(data->cs);
                for (int i = 0; i < data->size; i++) if (data->Arr[i] == data->markn) data->Arr[i] = 0;
                LeaveCriticalSection(data->cs);
            }
            
        }
        
    }
}

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
    Data* ArrData = new Data[nummark];
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
        );
    }
    


    InitializeCriticalSection(&cs);


    for (int i = 0; i < nummark; i++)
    {
        CloseHandle(Markers[i]);
        CloseHandle(MarkSignals[i]);
        CloseHandle(MainSignals[i]);
    }
    CloseHandle(StartEvent);
    DeleteCriticalSection(&cs);
    delete[] Arr, Markers, MarkSignals, MainSignals, ArrData;
}