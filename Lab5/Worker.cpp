#include <windows.h>
#include <iostream>
#include <string>
#include <cstdio>
#include "Common.h"

int main(int argc, char* argv[])
{   
    if (argc < 2)
    {
        std::cerr << "Run Worker.exe only via Browser.exe!" << std::endl;
        return 1;
    }

    int id = std::stoi(argv[1]);
    std::string title = "Web Worker ID: " + std::to_string(id);
    SetConsoleTitleA(title.c_str());

    std::cout << "Worker " << id << " started." << std::endl;

    std::string pipeInName = "\\\\.\\pipe\\worker_in_" + std::to_string(id);
    std::string pipeOutName = "\\\\.\\pipe\\worker_out_" + std::to_string(id);

    HANDLE hPipeIn = CreateFileA(
        pipeInName.c_str(),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    HANDLE hPipeOut = CreateFileA(
        pipeOutName.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hPipeIn == INVALID_HANDLE_VALUE || hPipeOut == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to connect to pipes. Error code: " << GetLastError() << std::endl;
        Sleep(5000);
        return 1;
    }

    std::cout << "Successfully connected to Browser." << std::endl;

    bool running = true;
    while (running)
    {
        Task task;
        DWORD bytesRead;

        std::cout << "Waiting for task..." << std::endl;

        BOOL success = ReadFile(
            hPipeIn,
            &task,
            sizeof(Task),
            &bytesRead,
            NULL
        );

        if (!success || bytesRead == 0)
        {
            std::cerr << "Pipe disconnected. Exiting." << std::endl;
            break;
        }

        if (task.type == TASK_EXIT)
        {
            std::cout << "Received exit command." << std::endl;
            running = false;
        }
        else if (task.type == TASK_COMPUTE)
        {
            std::cout << "Executing task ID: " << task.taskId 
                      << ". Sleeping " << task.sleepTime << " ms..." << std::endl;

            Sleep(task.sleepTime);

            
            Result res;
            res.workerId = id;
            res.taskId = task.taskId;
            res.success = true;
        
            sprintf_s(res.message, "Done in %d ms", task.sleepTime);

    
            DWORD bytesWritten;
            WriteFile(
                hPipeOut,
                &res,
                sizeof(Result),
                &bytesWritten,
                NULL
            );
            std::cout << "Result sent." << std::endl;
        }
    }

    
    CloseHandle(hPipeIn);
    CloseHandle(hPipeOut);
    
    std::cout << "Worker " << id << " finished work." << std::endl;
    Sleep(1000);
    return 0;
}