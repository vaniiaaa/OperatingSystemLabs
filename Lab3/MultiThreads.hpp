#ifndef MultiThreads_HPP
#define MultiThreads_HPP

#include <windows.h>
#include <iostream>

struct Data
{
    int *Arr;
    int size;
    int markn;
    int *numofmarked;
    HANDLE StartEvent, MarkSignal, MainSignal, ContinueEvent;
    CRITICAL_SECTION *cs;
};

DWORD WINAPI marker(LPVOID _data)
{
    int count = 0;
    Data *data = (Data *)_data;
    bool cont = true;
    std::srand(data->markn);
    while (cont)
    {
        WaitForSingleObject(data->StartEvent, INFINITE);
        EnterCriticalSection(data->cs);
        int index = rand() % data->size;
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
            std::cout << "Marker num: " << data->markn << " Num of changed pos: " << count << " Index of arr: " << index << '\n';
            *(data->numofmarked) += count;
            count = 0;
            SetEvent(data->MarkSignal);
            LeaveCriticalSection(data->cs);
            WaitForSingleObject(data->ContinueEvent, INFINITE);
            if (WaitForSingleObject(data->MainSignal, 0) == WAIT_OBJECT_0)
            {
                cont = false;
                EnterCriticalSection(data->cs);
                for (int i = 0; i < data->size; i++)
                    if (data->Arr[i] == data->markn)
                        data->Arr[i] = 0;
                data->numofmarked = 0;
                LeaveCriticalSection(data->cs);
            }
            else
            {
                continue;
            }
        }
    }
    return 0;
}

class MarkerRunner
{
    int size, nummark;
    int *numofmarked;
    int *Arr;
    HANDLE *MarkSignals;
    HANDLE *MainSignals;
    HANDLE *Markers;
    Data *ArrData;
    HANDLE StartEvent;
    HANDLE ContinueEvent;
    CRITICAL_SECTION cs;
    MarkerRunner() {};
    
    public:

    MarkerRunner(int _size, int _nummark) : size(_size), nummark(_nummark), numofmarked(0)
    {
        Arr = new int[size]();
        numofmarked = new int(0);
        MarkSignals = new HANDLE[nummark];
        MainSignals = new HANDLE[nummark];
        Markers = new HANDLE[nummark];
        ArrData = new Data[nummark];
        StartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        ContinueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        InitializeCriticalSection(&cs);
        for (int i = 0; i < nummark; i++)
        {
            MarkSignals[i] = CreateEvent(
                NULL,  // атрибуты защиты
                TRUE,  // тип события: TRUE - ручной (в несигнальное состояние только ResetEvent), FALSE (так же функциями ожидания)
                FALSE, // начальное состояние: TRUE - сигнальное
                NULL   // имя ивента для всех потоков
            );
            MainSignals[i] = CreateEvent(
                NULL,  // атрибуты защиты
                TRUE,  // тип события: TRUE - ручной (в несигнальное состояние только ResetEvent), FALSE (так же функциями ожидания)
                FALSE, // начальное состояние: TRUE - сигнальное
                NULL   // имя ивента для всех потоков
            );
            ArrData[i].Arr = Arr;
            ArrData[i].size = size;
            ArrData[i].markn = i + 1;
            ArrData[i].StartEvent = StartEvent;
            ArrData[i].ContinueEvent = ContinueEvent;
            ArrData[i].MainSignal = MainSignals[i];
            ArrData[i].MarkSignal = MarkSignals[i];
            ArrData[i].cs = &cs;
            ArrData[i].numofmarked = numofmarked;
            Markers[i] = CreateThread(
                NULL,
                0,
                marker,
                &ArrData[i],
                0,
                NULL);
        }
    }

    ~MarkerRunner()
    {
        for (int i = 0; i < nummark; i++)
        {
            CloseHandle(Markers[i]);
            CloseHandle(MarkSignals[i]);
            CloseHandle(MainSignals[i]);
        }
        CloseHandle(StartEvent);
        DeleteCriticalSection(&cs);
        delete[] Arr;
        delete[] Markers;
        delete[] MarkSignals;
        delete[] MainSignals;
        delete[] ArrData;
        delete numofmarked;
    }

    void StartMarkers()
    {
        SetEvent(StartEvent);
        ResetEvent(ContinueEvent);
    }

    void WaitForAllMarkers()
    {
        WaitForMultipleObjects(
            nummark, // колво объектов
            MarkSignals,
            TRUE,    // TRUE - ожидание всех в сигнальное, FALSE - любой
            INFINITE // время ожидания
        );
        ResetEvent(StartEvent);
    }

    void TerminateMarker(int mark)
    {
        SetEvent(MainSignals[mark - 1]);
        SetEvent(ContinueEvent);
        WaitForSingleObject(Markers[mark - 1], INFINITE);
    }

    void ContinueMarkers()
    {
        for (int i = 0; i < nummark; i++) {
            if (WaitForSingleObject(MainSignals[i], 0) != WAIT_OBJECT_0) {
                ResetEvent(MarkSignals[i]);
            }
        }
    }

    bool IsMarkerTerminated(int markerNum) const 
    {
        return WaitForSingleObject(MainSignals[markerNum - 1], 0) == WAIT_OBJECT_0;
    }

    bool AllMarkersTerminated() const 
    {
        return WaitForMultipleObjects(nummark, MainSignals, TRUE, 0) == WAIT_OBJECT_0;
    }

    int GetArrayValue(int index) const 
    {
        return Arr[index];
    }
    

    bool AllMarkedWith(int value) const 
    {
        for (int i = 0; i < size; i++)
        {
            if (Arr[i] != value) 
            {
                return false;
            }
        }
        return true;
    }
    
    int CountMarkedWith(int value) const 
    {
        int count = 0;
        for (int i = 0; i < size; i++) 
        {
            if (Arr[i] == value) 
            {
                count++;
            }
        }
        return count;
    }
    
    int GetSize() const 
    {
        return size;
    }
    
    int GetNumMarkers() const 
    {
        return nummark;
    }
    
    
    void PrintArray() const 
    {
        for (int i = 0; i < size; i++) 
        {
            std::cout << Arr[i] << " ";
        }
        std::cout << std::endl;
    }

    HANDLE* GetMainSignals() const
    {
        return MainSignals;
    }

    int GetNumOfMarked() const
    {
        return *numofmarked;
    }
};

#endif MultiThreads_HPP