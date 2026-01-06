#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Common.h"

struct WorkerInfo
{
    HANDLE hProcess;
    HANDLE hThread; 
    HANDLE hPipeIn; 
    HANDLE hPipeOut;
    int id;
};

int main()
{

    int nWorkers;
    int mTasks;

    std::cout << "Browser Manager" << std::endl;
    std::cout << "Enter number of workers (N): ";
    std::cin >> nWorkers;
    std::cout << "Enter number of tasks (M): ";
    std::cin >> mTasks;

    std::vector<WorkerInfo> workers;

    for (int i = 0; i < nWorkers; ++i)
    {
        WorkerInfo info;
        info.id = i;

        std::string pipeInName = "\\\\.\\pipe\\worker_in_" + std::to_string(i);
        std::string pipeOutName = "\\\\.\\pipe\\worker_out_" + std::to_string(i);

        info.hPipeIn = CreateNamedPipeA(
            pipeInName.c_str(),
            PIPE_ACCESS_OUTBOUND, 
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,                    
            sizeof(Task),         
            sizeof(Task),         
            0,
            NULL
        );

        
        info.hPipeOut = CreateNamedPipeA(
            pipeOutName.c_str(),
            PIPE_ACCESS_INBOUND,        
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            sizeof(Result),
            sizeof(Result),
            0,
            NULL
        );

        if (info.hPipeIn == INVALID_HANDLE_VALUE || info.hPipeOut == INVALID_HANDLE_VALUE)
        {
            std::cerr << "Error creating pipes for Worker " << i << std::endl;
            return 1;
        }

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::string cmdLine = "Worker.exe " + std::to_string(i);

        std::vector<char> cmdBuffer(cmdLine.begin(), cmdLine.end());
        cmdBuffer.push_back(0);

        if (!CreateProcessA(
            NULL,
            cmdBuffer.data(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        ))
        {
            std::cerr << "Error starting Worker.exe. Ensure the file is compiled." << std::endl;
            return 1;
        }

        info.hProcess = pi.hProcess;
        info.hThread = pi.hThread;
        workers.push_back(info);

        ConnectNamedPipe(info.hPipeIn, NULL);
        ConnectNamedPipe(info.hPipeOut, NULL);

        std::cout << "[Browser] Worker " << i << " started and connected." << std::endl;
    }

    std::cout << "Starting task distribution" << std::endl;

    for (int taskIdx = 0; taskIdx < mTasks; ++taskIdx)
    {
        int workerIdx = taskIdx % nWorkers;
        WorkerInfo& currentWorker = workers[workerIdx];

        Task task;
        task.type = TASK_COMPUTE;
        task.taskId = taskIdx;
        task.sleepTime = 500 + (rand() % 1000);

        std::cout << "[Browser] Sending task ID " << task.taskId 
                  << " (sleep " << task.sleepTime << "ms) to worker " << workerIdx << "..." << std::endl;
        
        DWORD bytesWritten;
        BOOL success = WriteFile(
            currentWorker.hPipeIn,
            &task,
            sizeof(Task),
            &bytesWritten,
            NULL
        );

        if (!success)
        {
            std::cerr << "[Error] Failed to send task to worker " << workerIdx << std::endl;
        }
        else
        {

            Result result;
            DWORD bytesRead;
            
            BOOL readSuccess = ReadFile(
                currentWorker.hPipeOut,
                &result,
                sizeof(Result),
                &bytesRead,
                NULL
            );

            if (readSuccess)
            {
                std::cout << "[Browser] Received response from Worker " << result.workerId 
                          << ": " << result.message << " (Task " << result.taskId << ")" << std::endl;
            }
        }
    }

    std::cout << "All tasks completed. Shutting down workers" << std::endl;

    for (int i = 0; i < nWorkers; ++i)
    {
        Task exitTask;
        exitTask.type = TASK_EXIT;
        exitTask.taskId = -1;
        exitTask.sleepTime = 0;

        DWORD bytesWritten;
        WriteFile(workers[i].hPipeIn, &exitTask, sizeof(Task), &bytesWritten, NULL);
    }

    for (int i = 0; i < nWorkers; ++i)
    {
        WaitForSingleObject(workers[i].hProcess, INFINITE);

        CloseHandle(workers[i].hPipeIn);
        CloseHandle(workers[i].hPipeOut);
        CloseHandle(workers[i].hProcess);
        CloseHandle(workers[i].hThread);
        
        std::cout << "[Browser] Worker " << i << " finished correctly." << std::endl;
    }

    std::cout << "Browser finished." << std::endl;
    system("pause");
    return 0;
}