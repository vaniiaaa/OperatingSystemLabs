#include <windows.h>
#include <iostream>


struct IncorrectInput: public std::exception {};

int main()
{
    int maxloads, files;
    std::cout << "Input max loads and count of files to download:\n";
    try
    {
        std::cin >> maxloads >> files;
        if (maxloads <= 0 || files < 0) throw IncorrectInput();
    }
    catch(const IncorrectInput)
    {
        std::cerr << "Incorrect Input: max loads must be positive number, files - not negative" << '\n';
        return 0;
    }
    
    HANDLE DownloadSlots = CreateSemaphoreA(
        NULL, // атрибуты защиты
        maxloads, // начальное значение семафора
        maxloads, // максимальное значение семафора
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
    ReleaseMutex(LogAccessMutex);
    HANDLE* processes = new HANDLE[files];
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;
    for (int i = 0; i < files; i++)
    {
        if (!CreateProcessA(
            NULL,                                                // Имя исполняемого модуля
            (LPSTR)"D:\\OperatingSystems\\Lab4\\downloader.exe", // Командная строка
            NULL,                                                // Атрибуты защиты процесса
            NULL,                                                // Атрибуты защиты потока
            FALSE,                                               // Наследование дескрипторов
            0,                                                   // Флаги создания
            NULL,                                                // Блок среды окружения
            NULL,                                                // Текущий каталог
            &si,                                                 // STARTUPINFO
            &pi))                                                // PROCESS_INFORMATION
        {
            std::cerr << "Processes wasn't created\n" << std::endl;
            CloseHandle(DownloadSlots);
            CloseHandle(LogAccessMutex);
            CloseHandle(BrowserClosingEvent);
            for (int i = 0; i < files; i++)
            {
                CloseHandle(processes[i]);
            }
            delete[] processes; 
            return 0;
        }
        processes[i] = pi.hProcess;
    }
    SetEvent(StartEvent);
    std::cout << "Browser is running. Press Enter to close...\n";
    std::cin.ignore();
    std::cin.get();
    SetEvent(BrowserClosingEvent);
    std::cout << "Browser is closing. Sending termination signal to all downloads...\n";
    WaitForMultipleObjects(files, processes, TRUE, INFINITE);
    std::cout << "All downloads finished. Exiting.\n";
    CloseHandle(DownloadSlots);
    CloseHandle(LogAccessMutex);
    CloseHandle(BrowserClosingEvent);
    CloseHandle(StartEvent);
    for (int i = 0; i < files; i++)
    {
        CloseHandle(processes[i]);
    }
    delete[] processes; 
}