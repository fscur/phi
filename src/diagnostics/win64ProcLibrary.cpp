#include <precompiled.h>

#include "win64ProcLibrary.h"

namespace phi
{
	win64ProcLibrary::dbgHelpSymInitilizeFunction win64ProcLibrary::symInitialize;
	win64ProcLibrary::dbgHelpStackWalk64Function win64ProcLibrary::stackWalk64;
	win64ProcLibrary::dbgHelpSymFunctionTableAccess64Function win64ProcLibrary::symFunctionTableAccess64;
	win64ProcLibrary::dbgHelpSymGetLineFromAddr64Function win64ProcLibrary::symGetLineFromAddr64;
	win64ProcLibrary::dbgHelpGetSymbolFromAddressFunction win64ProcLibrary::symGetSymFromAddr64;
	win64ProcLibrary::dbgHelpSymGetModuleBase64Function win64ProcLibrary::symGetModuleBase64;
	win64ProcLibrary::dbgHelpSymGetModuleInfo64Function win64ProcLibrary::symGetModuleInfo64;
	win64ProcLibrary::dbgHelpSymLoadModule64Function win64ProcLibrary::symLoadModule64;
	win64ProcLibrary::kernel32CreateToolhelp32SnapshotFunction win64ProcLibrary::createToolhelp32Snapshot;
	win64ProcLibrary::kernel32Module32FirstFunction win64ProcLibrary::module32First;
	win64ProcLibrary::kernel32Module32NextFunction win64ProcLibrary::module32Next;
	win64ProcLibrary::kernel32CloseHandleFunction win64ProcLibrary::closeHandle;
	win64ProcLibrary::kernel32CancelIoExFunction win64ProcLibrary::cancelIoEx;
}