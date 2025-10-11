#include <windows.h>
#include <ctime>
#include <iostream>
#include <string>


void ModeChild()
{
    int size;
    
    DWORD bytesRead;
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &size, sizeof(size), &bytesRead, NULL))
    {
        std::cerr << "Child: Error reading array size" << std::endl;
        return;
    }
    
    int* arr = new int[size];
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), arr, size * sizeof(int), &bytesRead, NULL))
    {
        std::cerr << "Child: Error reading array" << std::endl;
        delete[] arr;
        return;
    }
    
    double sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
    }
    double average = (size > 0) ? sum / size : 0;
    
    DWORD bytesWritten;
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &average, sizeof(average), &bytesWritten, NULL))
    {
        std::cerr << "Child: Error writing result" << std::endl;
    }
    
    delete[] arr;
}

void ModeParent()
{
    int size;
    std::cout << "Enter array size: ";
    std::cin >> size;
    
    if (size <= 0)
    {
        std::cout << "Invalid array size" << std::endl;
        return;
    }
    
    int* arr = new int[size];
    
    srand(static_cast<unsigned int>(time(nullptr)));
    std::cout << "Array: ";
    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % 100;
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    

    HANDLE hReadPipe1, hWritePipe1;
    HANDLE hReadPipe2, hWritePipe2; 

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    
    if (!CreatePipe(&hReadPipe1, &hWritePipe1, &sa, 0))
    {
        std::cerr << "Pipe 1 wasn't created" << std::endl;
        CloseHandle(hReadPipe1);
        CloseHandle(hWritePipe1);
        delete[] arr;
        return;
    }
    
    if (!CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0))
    {
        std::cerr << "Pipe 2 wasn't created" << std::endl;
        CloseHandle(hReadPipe2);
        CloseHandle(hWritePipe2);
        delete[] arr;
        return;
    }
    

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hReadPipe1; 
    si.hStdOutput = hWritePipe2;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
  
    char commandLine[] = "lab2.exe child";
    
    
    if (!CreateProcessA(
        NULL,                      // Имя исполняемого модуля
        commandLine, // Командная строка
        NULL,                      // Атрибуты защиты процесса
        NULL,                      // Атрибуты защиты потока
        TRUE,                      // Наследование дескрипторов
        0,                         // Флаги создания
        NULL,                      // Блок среды окружения
        NULL,                      // Текущий каталог
        &si,                       // STARTUPINFO
        &pi))                      // PROCESS_INFORMATION
    {
        std::cerr << "Process wasn't created" << std::endl;
        CloseHandle(hReadPipe1);
        CloseHandle(hWritePipe1);
        CloseHandle(hReadPipe2);
        CloseHandle(hWritePipe2);
        delete[] arr;
        return;
    }
    
    
    CloseHandle(hReadPipe1);
    CloseHandle(hWritePipe2);
    
    DWORD bytesWritten;
    
    if (!WriteFile(hWritePipe1, &size, sizeof(size), &bytesWritten, NULL))
    {
        std::cerr << "Error writing array size" << std::endl;
    }
    
    if (!WriteFile(hWritePipe1, arr, size * sizeof(int), &bytesWritten, NULL))
    {
        std::cerr << "Error writing array" << std::endl;
    }
    
    CloseHandle(hWritePipe1);
    
    double result;
    DWORD bytesRead;
    if (ReadFile(hReadPipe2, &result, sizeof(result), &bytesRead, NULL))
    {
        std::cout << "Average of array elements: " << result << std::endl;
    }
    else
    {
        std::cerr << "Error reading result from child process" << std::endl;
    }
    
   
    CloseHandle(hReadPipe2);
    

    WaitForSingleObject(pi.hProcess, INFINITE);
    
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    delete[] arr;
}

int main(int argc, char* argv[])
{
    if (argc > 1 && std::string(argv[1]) == "child")
    {
        ModeChild();
    }
    else
    {
        ModeParent();
    }
    
    return 0;
}