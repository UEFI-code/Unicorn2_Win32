#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include <stdio.h>

int Killer() 
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int num_cpu = sysinfo.dwNumberOfProcessors;
    printf("Number of CPUs: %d\n", num_cpu);

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!Process32First(hSnapShot, &pe)) {
        printf("Failed to enumerate processes.\n");
        return 1;
    }

    int unicorn_count = 0;
    do {
        if (strncmp(pe.szExeFile, "Unicorn2", 7) == 0) {
            unicorn_count++;
            if (unicorn_count > num_cpu * 2) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess != NULL) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    printf("Terminated process: %s\n", pe.szExeFile); 
                }
                else {
                    printf("Failed to terminate process: %s\n", pe.szExeFile);
                }
            }
        }
    } while (Process32Next(hSnapShot, &pe));

    CloseHandle(hSnapShot);
    return 0;
}