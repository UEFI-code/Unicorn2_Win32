#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include <stdio.h>
#include "Unicorn2.h"

HANDLE hSnapShot;
PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32), 0};

int get_pop_num()
{
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!Process32First(hSnapShot, &pe)) {
        printf("Failed to enumerate processes.\n");
        exit(1);
    }
    int unicorn_count = 0;
    do {
        if (wcsncmp(pe.szExeFile, L"Unicorn2", 8) == 0) {
            unicorn_count++;
        }
    } while (Process32Next(hSnapShot, &pe));
    CloseHandle(hSnapShot);
    return unicorn_count;
}

void kill_one()
{
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    Process32First(hSnapShot, &pe);
    do {
        if (wcsncmp(pe.szExeFile, L"Unicorn2", 8) == 0 && Get_Hardware_Rand() % 2 == 0) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
                printf("Killed unluck Unicorn2 process %d\n", pe.th32ProcessID);
                break;
            }
            else {
                printf("Failed to open process %d\n", pe.th32ProcessID);
            }
        }
    } while (Process32Next(hSnapShot, &pe));
    CloseHandle(hSnapShot);
}

int PopCtl() 
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int num_cpu = sysinfo.dwNumberOfProcessors;
    printf("Number of CPUs: %d\n", num_cpu);
    printf("Population control initialized.\n");
    while(1)
    {
        Sleep(1000);
        int pop_num = get_pop_num();
        printf("Number of Unicorn2 processes: %d\n", pop_num);
        if (pop_num < num_cpu * 2) {
            ShellExecuteA(NULL, "open", global_argv[0], NULL, NULL, SW_SHOWNORMAL);
        }
        else if (pop_num > 4 * num_cpu) {
            kill_one();
        }
    }
}