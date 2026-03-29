#include "ntdll_tools.h"
#include <stdio.h>

typedef struct _KSYSTEM_TIME
{
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;
#define PROCESSOR_FEATURE_MAX 64
typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE
{
    StandardDesign,
    NEC98x86,
    EndAlternatives
} ALTERNATIVE_ARCHITECTURE_TYPE;
typedef struct _KUSER_SHARED_DATA {
  ULONG                         TickCountLowDeprecated;
  ULONG                         TickCountMultiplier;
  KSYSTEM_TIME                  InterruptTime;
  KSYSTEM_TIME                  SystemTime;
  KSYSTEM_TIME                  TimeZoneBias;
  USHORT                        ImageNumberLow;
  USHORT                        ImageNumberHigh;
  WCHAR                         NtSystemRoot[260];
  ULONG                         MaxStackTraceDepth;
  ULONG                         CryptoExponent;
  ULONG                         TimeZoneId;
  ULONG                         LargePageMinimum;
  ULONG                         AitSamplingValue;
  ULONG                         AppCompatFlag;
  ULONGLONG                     RNGSeedVersion;
  ULONG                         GlobalValidationRunlevel;
  LONG                          TimeZoneBiasStamp;
  ULONG                         NtBuildNumber;
  NT_PRODUCT_TYPE               NtProductType;
  BOOLEAN                       ProductTypeIsValid;
  BOOLEAN                       Reserved0[1];
  USHORT                        NativeProcessorArchitecture;
  ULONG                         NtMajorVersion;
  ULONG                         NtMinorVersion;
  BOOLEAN                       ProcessorFeatures[PROCESSOR_FEATURE_MAX];
  ULONG                         Reserved1;
  ULONG                         Reserved3;
  ULONG                         TimeSlip;
  ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
  ULONG                         BootId;
  LARGE_INTEGER                 SystemExpirationDate;
  ULONG                         SuiteMask;
  BOOLEAN                       KdDebuggerEnabled;
  union {
    UCHAR MitigationPolicies;
    struct {
      UCHAR NXSupportPolicy : 2;
      UCHAR SEHValidationPolicy : 2;
      UCHAR CurDirDevicesSkippedForDlls : 2;
      UCHAR Reserved : 2;
    };
  };
  USHORT                        CyclesPerYield;
  ULONG                         ActiveConsoleId;
  ULONG                         DismountCount;
  ULONG                         ComPlusPackage;
  ULONG                         LastSystemRITEventTickCount;
  ULONG                         NumberOfPhysicalPages;
  BOOLEAN                       SafeBootMode;
  union {
    UCHAR VirtualizationFlags;
    struct {
      UCHAR ArchStartedInEl2 : 1;
      UCHAR QcSlIsSupported : 1;
    };
  };
  UCHAR                         Reserved12[2];
  union {
    ULONG SharedDataFlags;
    struct {
      ULONG DbgErrorPortPresent : 1;
      ULONG DbgElevationEnabled : 1;
      ULONG DbgVirtEnabled : 1;
      ULONG DbgInstallerDetectEnabled : 1;
      ULONG DbgLkgEnabled : 1;
      ULONG DbgDynProcessorEnabled : 1;
      ULONG DbgConsoleBrokerEnabled : 1;
      ULONG DbgSecureBootEnabled : 1;
      ULONG DbgMultiSessionSku : 1;
      ULONG DbgMultiUsersInSessionSku : 1;
      ULONG DbgStateSeparationEnabled : 1;
      ULONG SpareBits : 21;
    } DUMMYSTRUCTNAME2;
  } DUMMYUNIONNAME2;
  ULONG                         DataFlagsPad[1];
  ULONGLONG                     TestRetInstruction;
  LONGLONG                      QpcFrequency;
  ULONG                         SystemCall;
  ULONG                         Reserved2;
  ULONGLONG                     FullNumberOfPhysicalPages;
  ULONGLONG                     SystemCallPad[1];
  union {
    KSYSTEM_TIME TickCount;
    ULONG64      TickCountQuad;
    struct {
      ULONG ReservedTickCountOverlay[3];
      ULONG TickCountPad[1];
    } DUMMYSTRUCTNAME;
  } DUMMYUNIONNAME3;
  ULONG                         Cookie;
  ULONG                         CookiePad[1];
  LONGLONG                      ConsoleSessionForegroundProcessId;
  ULONGLONG                     TimeUpdateLock;
  ULONGLONG                     BaselineSystemTimeQpc;
  ULONGLONG                     BaselineInterruptTimeQpc;
  ULONGLONG                     QpcSystemTimeIncrement;
  ULONGLONG                     QpcInterruptTimeIncrement;
  UCHAR                         QpcSystemTimeIncrementShift;
  UCHAR                         QpcInterruptTimeIncrementShift;
  USHORT                        UnparkedProcessorCount;
  ULONG                         EnclaveFeatureMask[4];
  ULONG                         TelemetryCoverageRound;
  USHORT                        UserModeGlobalLogger[16];
  ULONG                         ImageFileExecutionOptions;
  ULONG                         LangGenerationCount;
  ULONGLONG                     Reserved4;
  ULONGLONG                     InterruptTimeBias;
  ULONGLONG                     QpcBias;
  ULONG                         ActiveProcessorCount;
  UCHAR                         ActiveGroupCount;
  UCHAR                         Reserved9;
  union {
    USHORT QpcData;
    struct {
      UCHAR QpcBypassEnabled;
      UCHAR QpcReserved;
    };
  };
//   LARGE_INTEGER                 TimeZoneBiasEffectiveStart;
//   LARGE_INTEGER                 TimeZoneBiasEffectiveEnd;
//   XSTATE_CONFIGURATION          XState;
//   KSYSTEM_TIME                  FeatureConfigurationChangeStamp;
//   ULONG                         Spare;
//   ULONG64                       UserPointerAuthMask;
//   XSTATE_CONFIGURATION          XStateArm64;
//   ULONG                         Reserved10[210];
} KUSER_SHARED_DATA, *PKUSER_SHARED_DATA;

PKUSER_SHARED_DATA SharedData = (PKUSER_SHARED_DATA)(0x7FFE0000);

void create_process(char *ascii_path)
{
    // assume ascii_path is like "\??\C:\Windows\System32\notepad.exe"
    if (strncmp(ascii_path, "\\??\\", 4) != 0)
    {
        printf("Invalid path format. Must start with \\??\\\n");
        return;
    }
    ANSI_STRING AnsiString;
    RtlInitAnsiString(&AnsiString, ascii_path);
    UNICODE_STRING nt_path;
    RtlAnsiStringToUnicodeString(&nt_path, &AnsiString, TRUE);
    UNICODE_STRING win32_path = { 0 };
    RtlInitUnicodeString(&win32_path, nt_path.Buffer + 4);
    UNICODE_STRING dll_path;
    RtlInitUnicodeString(&dll_path, SharedData->NtSystemRoot);
    PRTL_USER_PROCESS_PARAMETERS proc_param;
    WCHAR Env[2] = { 0, 0 };
    NTSTATUS status = RtlCreateProcessParameters(&proc_param, &win32_path, &dll_path, NULL, Env, NULL, &nt_path, 0, 0, 0);
    if (!NT_SUCCESS(status))
    {
        printf("RtlCreateProcessParameters failed: 0x%X\n", RtlNtStatusToDosError(status));
        return;
    }
	  wprintf(L"nt_path: %wZ\n", &nt_path);
    RTL_USER_PROCESS_INFORMATION proc_info = {0};
    status = RtlCreateUserProcess(
        &nt_path,
        OBJ_CASE_INSENSITIVE,
        proc_param,
        NULL,
        NULL,
        NULL,
        TRUE, // current cwd
        NULL,
        NULL,
        &proc_info
    );
    if (!NT_SUCCESS(status))
    {
        printf("RtlCreateUserProcess failed: 0x%X\n", RtlNtStatusToDosError(status));
        return;
    }
    switch (proc_info.ImageInformation.SubSystemType)
    {
    case 1:
        printf("Creating a Native type of process.\n");
        break;
    case 2:
        printf("Creating a GUI type of process.\n");
        break;
    case 3:
        printf("Creating a Console type of process.\n");
        break;
    default:
        printf("Creating an Unknown type of process.\n");
        break;
    }
    status = NtResumeThread(proc_info.ThreadHandle, NULL);
    if (!NT_SUCCESS(status))
    {
        printf("NtResumeThread failed: 0x%X\n", RtlNtStatusToDosError(status));
        return;
    }
    printf("Process created successfully. Handle: 0x%X\n", proc_info.ProcessHandle);
}