#include <Windows.h>
#include <iostream>
#include <process.h>
using namespace std;

struct ThreadData 
{
    int *arr;
    int size;
};

DWORD WINAPI worker(LPVOID data_)
{
    ThreadData* data = (ThreadData*)data_;
    int* arr = data->arr;
    int size = data->size;
    bool flag = 0;
    for (int i = 0; i < size; ++i)
    {
        if (arr[i] % 9 == 0) 
        {
            flag = 1;
            cout << arr[i] << endl;
        }
    }
    if (!flag) cout << "No elements" << endl;
    return 0;
}

unsigned __stdcall worker2(LPVOID data_) 
{
    ThreadData* data = (ThreadData*)data_;
    int* arr = data->arr;
    int size = data->size;
    bool flag = 0;
    for (int i = 0; i < size; ++i)
    {
        if (arr[i] % 9 == 0) 
        {
            flag = 1;
            cout << arr[i] << endl;
        }
    }
    if (!flag) cout << "No elements" << endl;
    return 0;
}

int main()
{
    int *arr;
    int size, timeout;
    cout << "Enter the size of array" << endl;
    cin >> size;
    arr = new int[size];
    cout << "Enter elements" << endl;
    for (int i = 0; i < size; i++)
    {
        cin >> arr[i];
    }
    cout << "Enter timeout" << endl;
    cin >> timeout;

    ThreadData* data = new ThreadData{arr, size};
    HANDLE hWorker1, hWorker2;
    DWORD IDWorker1;
    unsigned int IDWorker2;

    hWorker1 = CreateThread(NULL, 0, worker, data, CREATE_SUSPENDED, &IDWorker1);
    Sleep(timeout);
    ResumeThread(hWorker1);
    WaitForSingleObject(hWorker1, INFINITE);
    CloseHandle(hWorker1);

    hWorker2 = (HANDLE)_beginthreadex(NULL, 0, worker2, data, CREATE_SUSPENDED, &IDWorker2);
    Sleep(timeout);
    ResumeThread(hWorker2);
    WaitForSingleObject(hWorker2, INFINITE);
    CloseHandle(hWorker2);

    delete[] arr;
    delete data;
    return 0;
}