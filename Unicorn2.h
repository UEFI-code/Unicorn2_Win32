extern int NextPayloadSize;
extern UINT8* nextPayloadBuf;
extern int MuPos;
extern int MuWatchDog;

#define x86MaxInsLen 4
extern UINT8 BackupBuf[];

extern char** global_argv;

UINT64 Get_Hardware_Rand();