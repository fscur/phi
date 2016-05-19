#include <precompiled.h>

#include "windowsProcLoader.h"
#include "fileUtils.h"

namespace phi
{
	const char* windowsProcLoader::_dbgHelpLibraryName = "dbghelp.dll";
	const char* windowsProcLoader::_kernelLibraryName= "kernel32.dll";
	bool windowsProcLoader::_areLibrariesLoaded = false;
	windowsProcLoader::dbgHelpSymInitilizeFunction windowsProcLoader::symInitialize;
	windowsProcLoader::dbgHelpStackWalk64Function windowsProcLoader::stackWalk64;
	windowsProcLoader::dbgHelpSymFunctionTableAccess64Function windowsProcLoader::symFunctionTableAccess64;
	windowsProcLoader::dbgHelpSymGetLineFromAddr64Function windowsProcLoader::symGetLineFromAddr64;
	windowsProcLoader::dbgHelpGetSymbolFromAddressFunction windowsProcLoader::symGetSymFromAddr64;
	windowsProcLoader::dbgHelpSymGetModuleBase64Function windowsProcLoader::symGetModuleBase64;
	windowsProcLoader::dbgHelpSymGetModuleInfo64Function windowsProcLoader::symGetModuleInfo64;
	windowsProcLoader::dbgHelpSymLoadModule64Function windowsProcLoader::symLoadModule64;
	windowsProcLoader::kernel32CreateToolhelp32SnapshotFunction windowsProcLoader::createToolhelp32Snapshot;
	windowsProcLoader::kernel32Module32FirstFunction windowsProcLoader::module32First;
	windowsProcLoader::kernel32Module32NextFunction windowsProcLoader::module32Next;
	windowsProcLoader::kernel32CloseHandleFunction windowsProcLoader::closeHandle;
	windowsProcLoader::kernel32CancelIoExFunction windowsProcLoader::cancelIoEx;

	void windowsProcLoader::loadDbgHelpProcs(const char* executableFileName)
	{
		char dbgHelpPath[FILENAME_MAX] = { 0 };
		fileUtils::getDirectoryFromPath(executableFileName, dbgHelpPath);
		strcat_s(dbgHelpPath, _dbgHelpLibraryName);
		auto debugHelpLibrary = LoadLibrary(dbgHelpPath);

		symInitialize = (dbgHelpSymInitilizeFunction)GetProcAddress(debugHelpLibrary, "SymInitialize");
		stackWalk64 = (dbgHelpStackWalk64Function)GetProcAddress(debugHelpLibrary, "StackWalk64");
		symFunctionTableAccess64 = (dbgHelpSymFunctionTableAccess64Function)GetProcAddress(debugHelpLibrary, "SymFunctionTableAccess64");
		symGetLineFromAddr64 = (dbgHelpSymGetLineFromAddr64Function)GetProcAddress(debugHelpLibrary, "SymGetLineFromAddr64");
		symGetSymFromAddr64 = (dbgHelpGetSymbolFromAddressFunction)GetProcAddress(debugHelpLibrary, "SymGetSymFromAddr64");
		symGetModuleBase64 = (dbgHelpSymGetModuleBase64Function)GetProcAddress(debugHelpLibrary, "SymGetModuleBase64");
		symGetModuleInfo64 = (dbgHelpSymGetModuleInfo64Function)GetProcAddress(debugHelpLibrary, "SymGetModuleInfo64");
		symLoadModule64 = (dbgHelpSymLoadModule64Function)GetProcAddress(debugHelpLibrary, "SymLoadModule64");

	}

	void windowsProcLoader::loadKernel32Procs(const char* executableFileName)
	{
		auto kernel32Library = LoadLibrary(_kernelLibraryName);

		createToolhelp32Snapshot = (kernel32CreateToolhelp32SnapshotFunction)GetProcAddress(kernel32Library, "CreateToolhelp32Snapshot");
		module32First = (kernel32Module32FirstFunction)GetProcAddress(kernel32Library, "Module32First");
		module32Next = (kernel32Module32NextFunction)GetProcAddress(kernel32Library, "Module32Next");
		closeHandle = (kernel32CloseHandleFunction)GetProcAddress(kernel32Library, "CloseHandle");
		cancelIoEx = (kernel32CancelIoExFunction)GetProcAddress(kernel32Library, "CancelIoEx");
	}

	void windowsProcLoader::load(const char* executableFileName)
	{
		if (_areLibrariesLoaded)
			return;

		windowsProcLoader::loadDbgHelpProcs(executableFileName);
		windowsProcLoader::loadKernel32Procs(executableFileName);

		_areLibrariesLoaded = true;
	}

}