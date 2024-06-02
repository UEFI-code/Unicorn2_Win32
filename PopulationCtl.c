#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include <stdio.h>
#include <sys\timeb.h> 

int Killer() 
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int num_cpu = sysinfo.dwNumberOfProcessors;
    printf("Number of CPUs: %d\n", num_cpu);

    static struct timeb AccuTime;
    ftime(&AccuTime);
    srand((unsigned)AccuTime.time);

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!Process32First(hSnapShot, &pe)) {
        printf("Failed to enumerate processes.\n");
        return 1;
    }

    int unicorn_count = 0;
    do {
        if (wcsncmp(pe.szExeFile, L"Unicorn2", 8) == 0) {
            unicorn_count++;
        }
    } while (Process32Next(hSnapShot, &pe));

    printf("Number of Unicorn2 processes: %d\n", unicorn_count);

    if (unicorn_count > num_cpu * 2) {
        // Let's kill unlucky unicorns
        Process32First(hSnapShot, &pe);
        do {
            if (wcsncmp(pe.szExeFile, L"Unicorn2", 8) == 0 && rand() % 2 == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess != NULL) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    printf("Killed Unicorn2 process %d\n", pe.th32ProcessID);
                    unicorn_count--;
                    if (unicorn_count <= num_cpu * 2) {
                        break;
                    }
                }
                else {
                    printf("Failed to open process %d\n", pe.th32ProcessID);
                }
            }
        } while (Process32Next(hSnapShot, &pe));
    }
    
    CloseHandle(hSnapShot);
    return 0;
}