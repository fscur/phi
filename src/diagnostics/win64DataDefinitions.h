#pragma once

#include <phi.h>

namespace phi
{
    class win64DataDefinitions
    {
    public:
        typedef enum
        {
            SymNone = 0,
            SymCoff,
            SymCv,
            SymPdb,
            SymExport,
            SymDeferred,
            SymSym,
            SymDia,
            SymVirtual,
            NumSymTypes
        } symType;

        typedef enum
        {
            AddrMode1616,
            AddrMode1632,
            AddrModeReal,
            AddrModeFlat
        } addressMode;

        typedef struct _tagADDRESS64
        {
            DWORD64       Offset;
            WORD          Segment;
            addressMode  Mode;
        } address64, *lpaddress64;

        typedef struct _KDHELP64
        {
            DWORD64   Thread;
            DWORD   ThCallbackStack;
            DWORD   ThCallbackBStore;
            DWORD   NextCallback;
            DWORD   FramePointer;
            DWORD64   KiCallUserMode;
            DWORD64   KeUserCallbackDispatcher;
            DWORD64   SystemRangeStart;
            DWORD64   KiUserExceptionDispatcher;
            DWORD64   StackBase;
            DWORD64   StackLimit;
            DWORD     BuildVersion;
            DWORD     Reserved0;
            DWORD64   Reserved1[4];

        } kdhelp64, *pkdhelp64;

        struct symbol64
        {
            DWORD sizeOfStruct;
            DWORD64 address;
            DWORD size;
            DWORD flags;
            DWORD maxNameLength;
            CHAR name[1];
        };

        struct line64
        {
            DWORD sizeOfStruct;
            PVOID Key;
            DWORD lineNumber;
            PCHAR fileName;
            DWORD64 address;
        };

        struct module64
        {
            DWORD sizeOfStruct;
            DWORD64 baseOfImage;
            DWORD imageSize;
            DWORD timeDateStamp;
            DWORD checkSum;
            DWORD numSyms;
            symType symType;
            CHAR moduleName[32];
            CHAR ImageName[256];
            CHAR loadedImageName[256];
            CHAR loadedPdbName[256];
            DWORD cvSig;
            CHAR cvData[MAX_PATH * 3];
            DWORD pdbSig;
            GUID pdbSig70;
            DWORD pdbAge;
            BOOL pdbUnmatched;
            BOOL dbgUnmatched;
            BOOL lineNumbers;
            BOOL globalSymbols;
            BOOL typeInfo;
            BOOL sourceIndexed;
            BOOL publics;
            DWORD machineType;
            DWORD reserved;
        };

        struct moduleEntry32
        {
            DWORD dwSize;
            DWORD th32ModuleID;
            DWORD th32ProcessID;
            DWORD GlblcntUsage;
            DWORD ProccntUsage;
            BYTE * modBaseAddr;
            DWORD modBaseSize;
            HMODULE hModule;
            char szModule[256];
            char szExePath[MAX_PATH];
        };

        struct stackFrame
        {
            address64 AddrPC;
            address64 AddrReturn;
            address64 AddrFrame;
            address64 AddrStack;
            address64 AddrBStore;
            PVOID FuncTableEntry;
            DWORD64 Params[4];
            BOOL Far;
            BOOL Virtual;
            DWORD64 Reserved[3];
            kdhelp64 KdHelp;
        };

        static const int MAX_MODULE_NAME32 = 255;

        static const DWORD TH32CS_SNAPHEAPLIST = 0x00000001;
        static const DWORD TH32CS_SNAPPROCESS = 0x00000002;
        static const DWORD TH32CS_SNAPTHREAD = 0x00000004;
        static const DWORD TH32CS_SNAPMODULE = 0x00000008;
        static const DWORD TH32CS_SNAPMODULE32 = 0x00000010;
        static const DWORD TH32CS_SNAPALL = (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE);
        static const DWORD TH32CS_INHERIT = 0x80000000;

        static const DWORD SNAPHEAPLIST = 0x00000001;
        static const DWORD SNAPPROCESS = 0x00000002;
        static const DWORD SNAPTHREAD = 0x00000004;
        static const DWORD SNAPMODULE = 0x00000008;
        static const DWORD SNAPMODULE32 = 0x00000010;
        static const DWORD SNAPALL = (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE);
        static const DWORD INHERIT = 0x80000000;
    };
}