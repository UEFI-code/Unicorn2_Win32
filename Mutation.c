#include <stdio.h>
#include <Windows.h>

#include "Unicorn2.h"

UINT8 BackupBuf[x86MaxInsLen] = { 0x0 };

void MuNxtPayload()
{
    while(1)
    {
        printf("Finding Liveable Mutation Position...\n");
        MuPos = Get_Hardware_Rand() % (NextPayloadSize - 5 - x86MaxInsLen) + 4; // Keep first 4 bytes and last 1 byte
        for(int i=0; i<x86MaxInsLen; i++)
        {
            BackupBuf[i] = nextPayloadBuf[MuPos + i];
            nextPayloadBuf[MuPos + i] = Get_Hardware_Rand() % 256;
        }
        __try
        {
            ((void(*)())(nextPayloadBuf + 1))();
            printf("Mutation Success @ 0x%X\n", MuPos);
            break;
        }
        __except(TRUE)
        {
            // printf("Mutation Failed @ 0x%X, revert x86 code\n", MuPos);
            MuWatchDog = time(NULL);
            for(int i=0; i<x86MaxInsLen; i++)
            {
                nextPayloadBuf[MuPos + i] = BackupBuf[i];
            }
        }
    }
}