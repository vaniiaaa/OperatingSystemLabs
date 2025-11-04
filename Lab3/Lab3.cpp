#include <windows.h>
#include <iostream>


CRITICAL_SECTION cs;

struct Data
{
    int* Arr;
    int size;
    int markn;
    HANDLE StartEvent, MarkSignal, MainSignal, ContinueEvent;
    CRITICAL_SECTION* cs;
};

DWORD WINAPI marker(LPVOID _data)
{
    Data* data = (Data*)_data;
    int count = 0 ;
    bool cont = true;
    std::srand(data->markn);
    while(cont) {
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
            count = 0;
            SetEvent(data->MarkSignal);
            LeaveCriticalSection(data->cs);
            WaitForSingleObject(data->ContinueEvent, INFINITE); 
            if (WaitForSingleObject(data->MainSignal, 0) == WAIT_OBJECT_0)
            {
                cont = false;
                EnterCriticalSection(data->cs);
                for (int i = 0; i < data->size; i++) if (data->Arr[i] == data->markn) data->Arr[i] = 0;
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

class InvalidBoundInput : public std::exception{};
class InvalidSecondInput : public std::exception{};

void InputController(int& input, int lower, int upper, HANDLE* MainSignals)
{
    while (true)
    {
        try 
        {
            std::cin >> input;
            if (lower > input || input > upper) throw InvalidBoundInput();
            if (WaitForSingleObject(MainSignals[input - 1], 0) == WAIT_OBJECT_0) throw InvalidSecondInput();
            break;
        }
        catch (const InvalidBoundInput& e)
        {
            std::cout << "Value have to be between " << lower + 1 << " and " << upper << '\n';
        }
        catch (const InvalidSecondInput& e)
        {
            std::cout << "Marker " << input << " already terminated" << '\n';
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
    HANDLE ContinueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
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
        ArrData[i].ContinueEvent = ContinueEvent;
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
    
    while(true)
    {
        SetEvent(StartEvent);
        ResetEvent(ContinueEvent);
        WaitForMultipleObjects(
            nummark, //колво объектов
            MarkSignals,
            TRUE, //TRUE - ожидание всех в сигнальное, FALSE - любой
            INFINITE  //время ожидания
        );
        ResetEvent(StartEvent);
        for (int i = 0; i < size; i++) std::cout << Arr[i] << " ";
        std::cout << '\n' << "Input num of marker to terminate: ";
        int mark;
        InputController(mark, 0, nummark, MainSignals);
        SetEvent(MainSignals[mark - 1]);
        SetEvent(ContinueEvent);
        WaitForSingleObject(Markers[mark - 1], INFINITE);
        for (int i = 0; i < size; i++) std::cout << Arr[i] << " ";
        std::cout << '\n';
        for (int i = 0; i < nummark; i++) {
            if (WaitForSingleObject(MainSignals[i], 0) == WAIT_OBJECT_0) continue;
            else ResetEvent(MarkSignals[i]);
        }
        if(WaitForMultipleObjects(nummark, MainSignals, TRUE, 0) == WAIT_OBJECT_0) break;
    }


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