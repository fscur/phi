#pragma once
#include <phi.h>

namespace phi
{
    class debugHelp
    {
    private:
        static bool _areLibrariesLoaded;
    public:
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

        struct module64 {
            DWORD sizeOfStruct;
            DWORD64 baseOfImage;
            DWORD imageSize;
            DWORD timeDateStamp;
            DWORD checkSum;
            DWORD numSyms;
            SYM_TYPE symType;
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
            char szModule[MAX_MODULE_NAME32 + 1];
            char szExePath[MAX_PATH];
        };

        struct stackFrame
        {
            ADDRESS64 AddrPC;
            ADDRESS64 AddrReturn;
            ADDRESS64 AddrFrame;
            ADDRESS64 AddrStack;
            ADDRESS64 AddrBStore;
            PVOID FuncTableEntry;
            DWORD64 Params[4];
            BOOL Far;
            BOOL Virtual;
            DWORD64 Reserved[3];
            KDHELP64 KdHelp;
        };

    public:
        typedef BOOL(__stdcall *symInitilizeFunction)(
            _In_ HANDLE process,
            _In_opt_ PCSTR userSearchPath,
            _In_ BOOL invadeProcess);

        typedef BOOL(__stdcall *readProcessMemoryRoutine64)(
            _In_ HANDLE process,
            _In_ DWORD64 qwBaseAddress,
            _Out_writes_bytes_(nSize) PVOID lpBuffer,
            _In_ DWORD nSize,
            _Out_ LPDWORD lpNumberOfBytesRead);

        typedef PVOID(__stdcall *symFunctionTableAccess64Function)(
            _In_ HANDLE process,
            _In_ DWORD64 addrBase);

        typedef DWORD64(__stdcall *symGetModuleBase64Function)(
            _In_ HANDLE process,
            _In_ DWORD64 qwAddr);

        typedef DWORD64 (__stdcall *tranlateAddressRoutine64)(
                _In_ HANDLE process,
                _In_ HANDLE thread,
                _In_ LPADDRESS64 lpaddr);

        typedef BOOL(__stdcall *stackWalk64Function)(
            _In_ DWORD machineType,
            _In_ HANDLE process,
            _In_ HANDLE thread,
            _Inout_ stackFrame* stackFrame,
            _Inout_ PVOID contextRecord,
            _In_opt_ readProcessMemoryRoutine64 readMemoryRoutine,
            _In_opt_ symFunctionTableAccess64Function functionTableAccessRoutine,
            _In_opt_ symGetModuleBase64Function getModuleBaseRoutine,
            _In_opt_ tranlateAddressRoutine64 translateAddress);

        typedef BOOL(__stdcall *symGetLineFromAddr64Function)(
            _In_ HANDLE process,
            _In_ DWORD64 qwAddr,
            _Out_ PDWORD pdwDisplacement,
            _Out_ line64* line64);

        typedef BOOL(__stdcall *getSymbolFromAddressFunction)(
            _In_ HANDLE process,
            _In_ DWORD64 address,
            _Out_ PDWORD64 displacement,
            _Out_ symbol64* symbol);

        typedef BOOL(__stdcall *symGetModuleInfo64Function)(
            _In_ HANDLE hProcess,
            _In_ DWORD64 qwAddr,
            _Out_ module64* moduleInfo);

        typedef DWORD64(__stdcall *symLoadModule64Function)(
            _In_ HANDLE process,
            _In_opt_ HANDLE fileHandle,
            _In_opt_ PCSTR imageName,
            _In_opt_ PCSTR moduleName,
            _In_ DWORD64 baseOfDll,
            _In_ DWORD sizeOfDll);

        typedef HANDLE(__stdcall *createToolhelp32SnapshotFunction)(
            DWORD flags,
            DWORD th32ProcessID);

        typedef BOOL(__stdcall *module32FirstFunction)(
            HANDLE hSnapshot,
            moduleEntry32* lpme);

        typedef BOOL(__stdcall *module32NextFunction)(
            HANDLE hSnapshot,
            moduleEntry32* lpme);

        typedef BOOL(__stdcall *closeHandleFunction)(_In_ HANDLE handle);

        static const DWORD SNAPHEAPLIST = 0x00000001;
        static const DWORD SNAPPROCESS = 0x00000002;
        static const DWORD SNAPTHREAD = 0x00000004;
        static const DWORD SNAPMODULE = 0x00000008;
        static const DWORD SNAPMODULE32 = 0x00000010;
        static const DWORD SNAPALL = (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE);
        static const DWORD INHERIT = 0x80000000;
    public:
        static symInitilizeFunction symInitialize;
        static stackWalk64Function stackWalk64;
        static symFunctionTableAccess64Function symFunctionTableAccess64;
        static symGetLineFromAddr64Function symGetLineFromAddr64;
        static getSymbolFromAddressFunction symGetSymFromAddr64;
        static symGetModuleBase64Function symGetModuleBase64;
        static symGetModuleInfo64Function symGetModuleInfo64;
        static symLoadModule64Function symLoadModule64;

        static createToolhelp32SnapshotFunction createToolhelp32Snapshot;
        static module32FirstFunction module32First;
        static module32NextFunction module32Next;
        static closeHandleFunction closeHandle;

        static void load(const char* executableFileName);
    };
}