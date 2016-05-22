#include <precompiled.h>
#include "debugHelp.h"
#include "fileUtils.h"

namespace phi
{
    bool debugHelp::_areLibrariesLoaded = false;

    debugHelp::symInitilizeFunction debugHelp::symInitialize;
    debugHelp::stackWalk64Function debugHelp::stackWalk64;
    debugHelp::symFunctionTableAccess64Function debugHelp::symFunctionTableAccess64;
    debugHelp::symGetLineFromAddr64Function debugHelp::symGetLineFromAddr64;
    debugHelp::getSymbolFromAddressFunction debugHelp::symGetSymFromAddr64;
    debugHelp::symGetModuleBase64Function debugHelp::symGetModuleBase64;
    debugHelp::symGetModuleInfo64Function debugHelp::symGetModuleInfo64;
    debugHelp::symLoadModule64Function debugHelp::symLoadModule64;

    debugHelp::createToolhelp32SnapshotFunction debugHelp::createToolhelp32Snapshot;
    debugHelp::module32FirstFunction debugHelp::module32First;
    debugHelp::module32NextFunction debugHelp::module32Next;
    debugHelp::closeHandleFunction debugHelp::closeHandle;

    void debugHelp::load(const char* executableFileName)
    {
        if (_areLibrariesLoaded)
            return;

        const char* dbgHelpLibraryName = "dbghelp.dll";
        const char* kernelLibraryName = "kernel32.dll";

        char dbgHelpPath[FILENAME_MAX] = { 0 };
        fileUtils::getDirectoryFromPath(executableFileName, dbgHelpPath);
        strcat_s(dbgHelpPath, dbgHelpLibraryName);

        auto debugHelpLibrary = LoadLibrary(dbgHelpPath);
        auto kernel32Library = LoadLibrary(kernelLibraryName);

        symInitialize = (symInitilizeFunction)GetProcAddress(debugHelpLibrary, "SymInitialize");
        stackWalk64 = (stackWalk64Function)GetProcAddress(debugHelpLibrary, "StackWalk64");
        symFunctionTableAccess64 = (symFunctionTableAccess64Function)GetProcAddress(debugHelpLibrary, "SymFunctionTableAccess64");
        symGetLineFromAddr64 = (symGetLineFromAddr64Function)GetProcAddress(debugHelpLibrary, "SymGetLineFromAddr64");
        symGetSymFromAddr64 = (getSymbolFromAddressFunction)GetProcAddress(debugHelpLibrary, "SymGetSymFromAddr64");
        symGetModuleBase64 = (symGetModuleBase64Function)GetProcAddress(debugHelpLibrary, "SymGetModuleBase64");
        symGetModuleInfo64 = (symGetModuleInfo64Function)GetProcAddress(debugHelpLibrary, "SymGetModuleInfo64");
        symLoadModule64 = (symLoadModule64Function)GetProcAddress(debugHelpLibrary, "SymLoadModule64");

        createToolhelp32Snapshot = (createToolhelp32SnapshotFunction)GetProcAddress(kernel32Library, "CreateToolhelp32Snapshot");
        module32First = (module32FirstFunction)GetProcAddress(kernel32Library, "Module32First");
        module32Next = (module32NextFunction)GetProcAddress(kernel32Library, "Module32Next");
        closeHandle = (closeHandleFunction)GetProcAddress(kernel32Library, "CloseHandle");

        _areLibrariesLoaded = true;
    }
}
