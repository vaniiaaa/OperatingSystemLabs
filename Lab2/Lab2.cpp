#include <windows.h>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

void ModeChild()
{
    int size;
    
    DWORD bytesRead;
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &size, sizeof(size), &bytesRead, NULL))
    {
        cerr << "Child: Error reading array size" << endl;
        return;
    }
    
    int* arr = new int[size];
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), arr, size * sizeof(int), &bytesRead, NULL))
    {
        cerr << "Child: Error reading array" << endl;
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
        cerr << "Child: Error writing result" << endl;
    }
    
    delete[] arr;
}

void ModeParent()
{
    int size;
    cout << "Enter array size: ";
    cin >> size;
    
    if (size <= 0)
    {
        cout << "Invalid array size" << endl;
        return;
    }
    
    int* arr = new int[size];
    
    srand(static_cast<unsigned int>(time(nullptr)));
    cout << "Array: ";
    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % 100;
        cout << arr[i] << " ";
    }
    cout << endl;
    

    HANDLE hReadPipe1, hWritePipe1;
    HANDLE hReadPipe2, hWritePipe2; 

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    
    if (!CreatePipe(&hReadPipe1, &hWritePipe1, &sa, 0))
    {
        cerr << "Pipe 1 wasn't created" << endl;
        CloseHandle(hReadPipe1);
        CloseHandle(hWritePipe1);
        delete[] arr;
        return;
    }
    
    if (!CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0))
    {
        cerr << "Pipe 2 wasn't created" << endl;
        CloseHandle(hReadPipe2);
        CloseHandle(hWritePipe2);
        delete[] arr;
        return;
    }
    

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hReadPipe1; 
    si.hStdOutput = hWritePipe2;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
  
    string commandLine = "lab2.exe child";
    
    
    if (!CreateProcess(
        NULL,                      // Имя исполняемого модуля
        const_cast<LPSTR>(commandLine.c_str()), // Командная строка
        NULL,                      // Атрибуты защиты процесса
        NULL,                      // Атрибуты защиты потока
        TRUE,                      // Наследование дескрипторов
        0,                         // Флаги создания
        NULL,                      // Блок среды окружения
        NULL,                      // Текущий каталог
        &si,                       // STARTUPINFO
        &pi))                      // PROCESS_INFORMATION
    {
        cerr << "Process wasn't created" << endl;
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
        cerr << "Error writing array size" << endl;
    }
    
    if (!WriteFile(hWritePipe1, arr, size * sizeof(int), &bytesWritten, NULL))
    {
        cerr << "Error writing array" << endl;
    }
    
    CloseHandle(hWritePipe1);
    
    double result;
    DWORD bytesRead;
    if (ReadFile(hReadPipe2, &result, sizeof(result), &bytesRead, NULL))
    {
        cout << "Average of array elements: " << result << endl;
    }
    else
    {
        cerr << "Error reading result from child process" << endl;
    }
    
   
    CloseHandle(hReadPipe2);
    

    WaitForSingleObject(pi.hProcess, INFINITE);
    
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    delete[] arr;
}

int main(int argc, char* argv[])
{
    if (argc > 1 && string(argv[1]) == "child")
    {
        ModeChild();
    }
    else
    {
        ModeParent();
    }
    
    return 0;
}