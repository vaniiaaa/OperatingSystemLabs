#include <windows.h>
#include <iostream>
#include <ctime>
#include <fstream>

double work()
{
    std::fstream in("input.txt");
    double res = 1;
    for (int i = 0; i < 100; i++)
    {
        double temp;
        in >> temp;
        res *= temp;
    }
    return res;
}

int main()
{
    HANDLE handl[2];
    double res;
    HANDLE DownloadSlots = CreateSemaphoreA(
        NULL, // атрибуты защиты
        0, // начальное значение семафора
        1, // максимальное значение семафора
        "DownloadSlots" // имя семафора
        );
    HANDLE LogAccessMutex = CreateMutexA(
        NULL, // атрибуты защиты
        TRUE, // начальный владелец мьютекса TRUE - принадлежит сразу создателю 
        "LogAccessMutex" // имя мьютекса
        );
    HANDLE BrowserClosingEvent = CreateEventA(
        NULL,  // атрибуты защиты
        TRUE,  // тип события: TRUE - ручной (в несигнальное состояние только ResetEvent), FALSE (так же функциями ожидания)
        FALSE, // начальное состояние: TRUE - сигнальное
        "BrowserClosingEvent"   // имя ивента для всех потоков
        );
    HANDLE StartEvent = CreateEventA(
        NULL,
        TRUE,
        FALSE,
        "StartEvent"
        );
    WaitForSingleObject(StartEvent, INFINITE); 
    handl[0] = DownloadSlots;
    handl[1] = BrowserClosingEvent;
    DWORD waitresult = WaitForMultipleObjects(2, handl, FALSE, INFINITE);
    switch (waitresult)
    {
    case WAIT_OBJECT_0 + 0:
        WaitForSingleObject(LogAccessMutex, INFINITE);
            std::cout << "[PID: " << GetCurrentProcessId() << "] Connection established. Starting download of 'file_name.ext'... \n";
        ReleaseMutex(LogAccessMutex);

        work();
        srand(time(0));
        Sleep((rand() % 2001) + 1000);

        WaitForSingleObject(LogAccessMutex, INFINITE);
            std::cout <<  "[PID: " << GetCurrentProcessId() << "] File 'file_name.ext' processed successfully. \n";
            ReleaseMutex(LogAccessMutex);
        ReleaseSemaphore(DownloadSlots, 1, NULL);
        break;
    case WAIT_OBJECT_0 + 1:
        WaitForSingleObject(LogAccessMutex, INFINITE);
            std::cout << "[PID: " << GetCurrentProcessId() << "] Connection was lost\n";
        ReleaseMutex(LogAccessMutex);
        break;
    default:
        break;
    }
        

    CloseHandle(DownloadSlots);
    CloseHandle(LogAccessMutex);
    CloseHandle(BrowserClosingEvent);
    CloseHandle(handl[0]);
    CloseHandle(handl[1]);
}
