#pragma once

enum TaskType
{
    TASK_COMPUTE,
    TASK_EXIT    
};

struct Task
{
    TaskType type;  
    int taskId;     
    int sleepTime;  
};


struct Result
{
    int workerId;    
    int taskId;      
    bool success;    
    char message[256];
};