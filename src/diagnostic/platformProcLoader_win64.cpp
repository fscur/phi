#include <precompiled.h>
#include "platformProcLoader.h"
#include "fileUtils.h"
#include "win64ProcLibrary.h"

namespace phi
{
    bool platformProcLoader::_procsAreLoaded = false;
    const char* platformProcLoader::_dbgHelpLibraryName = "dbghelp.dll";
    const char* platformProcLoader::_kernelLibraryName = "kernel32.dll";

    void platformProcLoader::loadDbgHelpProcs(const char* executableFileName)
    {
        char dbgHelpPath[FILENAME_MAX] = { 0 };
        fileUtils::getDirectoryFromPath(executableFileName, dbgHelpPath);
        strcat_s(dbgHelpPath, _dbgHelpLibraryName);
        auto debugHelpLibrary = LoadLibrary(dbgHelpPath);

        win64ProcLibrary::symInitialize = (win64ProcLibrary::dbgHelpSymInitilizeFunction)GetProcAddress(debugHelpLibrary, "SymInitialize");
        win64ProcLibrary::stackWalk64 = (win64ProcLibrary::dbgHelpStackWalk64Function)GetProcAddress(debugHelpLibrary, "StackWalk64");
        win64ProcLibrary::symFunctionTableAccess64 = (win64ProcLibrary::dbgHelpSymFunctionTableAccess64Function)GetProcAddress(debugHelpLibrary, "SymFunctionTableAccess64");
        win64ProcLibrary::symGetLineFromAddr64 = (win64ProcLibrary::dbgHelpSymGetLineFromAddr64Function)GetProcAddress(debugHelpLibrary, "SymGetLineFromAddr64");
        win64ProcLibrary::symGetSymFromAddr64 = (win64ProcLibrary::dbgHelpGetSymbolFromAddressFunction)GetProcAddress(debugHelpLibrary, "SymGetSymFromAddr64");
        win64ProcLibrary::symGetModuleBase64 = (win64ProcLibrary::dbgHelpSymGetModuleBase64Function)GetProcAddress(debugHelpLibrary, "SymGetModuleBase64");
        win64ProcLibrary::symGetModuleInfo64 = (win64ProcLibrary::dbgHelpSymGetModuleInfo64Function)GetProcAddress(debugHelpLibrary, "SymGetModuleInfo64");
        win64ProcLibrary::symLoadModule64 = (win64ProcLibrary::dbgHelpSymLoadModule64Function)GetProcAddress(debugHelpLibrary, "SymLoadModule64");
    }

    void platformProcLoader::loadKernel32Procs(const char* executableFileName)
    {
        _unused(executableFileName);

        auto kernel32Library = LoadLibrary(_kernelLibraryName);

        win64ProcLibrary::createToolhelp32Snapshot = (win64ProcLibrary::kernel32CreateToolhelp32SnapshotFunction)GetProcAddress(kernel32Library, "CreateToolhelp32Snapshot");
        win64ProcLibrary::module32First = (win64ProcLibrary::kernel32Module32FirstFunction)GetProcAddress(kernel32Library, "Module32First");
        win64ProcLibrary::module32Next = (win64ProcLibrary::kernel32Module32NextFunction)GetProcAddress(kernel32Library, "Module32Next");
        win64ProcLibrary::closeHandle = (win64ProcLibrary::kernel32CloseHandleFunction)GetProcAddress(kernel32Library, "CloseHandle");
        win64ProcLibrary::cancelIoEx = (win64ProcLibrary::kernel32CancelIoExFunction)GetProcAddress(kernel32Library, "CancelIoEx");
    }

    void platformProcLoader::load(const char* executableFileName)
    {
        if (!_procsAreLoaded)
        {
            platformProcLoader::loadDbgHelpProcs(executableFileName);
            platformProcLoader::loadKernel32Procs(executableFileName);
            _procsAreLoaded = true;
        }
    }
}