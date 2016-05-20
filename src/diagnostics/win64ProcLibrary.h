#pragma once
#include <phi.h>

#include "diagnosticsApi.h"
#include "win64DataDefinitions.h"

namespace phi
{
	class win64ProcLibrary
	{	
	public:
		typedef BOOL(__stdcall *dbgHelpSymInitilizeFunction)(
			_In_ HANDLE process,
			_In_opt_ PCSTR userSearchPath,
			_In_ BOOL invadeProcess);

		typedef BOOL(__stdcall *dbgHelpReadProcessMemoryRoutine64)(
			_In_ HANDLE process,
			_In_ DWORD64 qwBaseAddress,
			_Out_writes_bytes_(nSize) PVOID lpBuffer,
			_In_ DWORD nSize,
			_Out_ LPDWORD lpNumberOfBytesRead);

		typedef PVOID(__stdcall *dbgHelpSymFunctionTableAccess64Function)(
			_In_ HANDLE process,
			_In_ DWORD64 addrBase);

		typedef DWORD64(__stdcall *dbgHelpSymGetModuleBase64Function)(
			_In_ HANDLE process,
			_In_ DWORD64 qwAddr);

		typedef DWORD64(__stdcall *dbgHelpTranlateAddressRoutine64)(
			_In_ HANDLE process,
			_In_ HANDLE thread,
			_In_ win64DataDefinitions::lpaddress64 lpaddr);

		typedef BOOL(__stdcall *dbgHelpStackWalk64Function)(
			_In_ DWORD machineType,
			_In_ HANDLE process,
			_In_ HANDLE thread,
			_Inout_ win64DataDefinitions::stackFrame* stackFrame,
			_Inout_ PVOID contextRecord,
			_In_opt_ dbgHelpReadProcessMemoryRoutine64 readMemoryRoutine,
			_In_opt_ dbgHelpSymFunctionTableAccess64Function functionTableAccessRoutine,
			_In_opt_ dbgHelpSymGetModuleBase64Function getModuleBaseRoutine,
			_In_opt_ dbgHelpTranlateAddressRoutine64 translateAddress);

		typedef BOOL(__stdcall *dbgHelpSymGetLineFromAddr64Function)(
			_In_ HANDLE process,
			_In_ DWORD64 qwAddr,
			_Out_ PDWORD pdwDisplacement,
			_Out_ win64DataDefinitions::line64* line64);

		typedef BOOL(__stdcall *dbgHelpGetSymbolFromAddressFunction)(
			_In_ HANDLE process,
			_In_ DWORD64 address,
			_Out_ PDWORD64 displacement,
			_Out_ win64DataDefinitions::symbol64* symbol);

		typedef BOOL(__stdcall *dbgHelpSymGetModuleInfo64Function)(
			_In_ HANDLE hProcess,
			_In_ DWORD64 qwAddr,
			_Out_ win64DataDefinitions::module64* moduleInfo);

		typedef DWORD64(__stdcall *dbgHelpSymLoadModule64Function)(
			_In_ HANDLE process,
			_In_opt_ HANDLE fileHandle,
			_In_opt_ PCSTR imageName,
			_In_opt_ PCSTR moduleName,
			_In_ DWORD64 baseOfDll,
			_In_ DWORD sizeOfDll);

		typedef HANDLE(__stdcall *kernel32CreateToolhelp32SnapshotFunction)(
			DWORD flags,
			DWORD th32ProcessID);

		typedef BOOL(__stdcall *kernel32Module32FirstFunction)(
			HANDLE hSnapshot,
			win64DataDefinitions::moduleEntry32* lpme);

		typedef BOOL(__stdcall *kernel32Module32NextFunction)(
			HANDLE hSnapshot,
			win64DataDefinitions::moduleEntry32* lpme);

		typedef BOOL(__stdcall *kernel32CloseHandleFunction)(
			_In_ HANDLE handle);

		typedef BOOL(__stdcall *kernel32CancelIoExFunction)(
			_In_ HANDLE hFile,
			_In_opt_ LPOVERLAPPED lpOverlapped);

		DIAGNOSTICS_API static dbgHelpSymInitilizeFunction symInitialize;
		DIAGNOSTICS_API static dbgHelpStackWalk64Function stackWalk64;
		DIAGNOSTICS_API static dbgHelpSymFunctionTableAccess64Function symFunctionTableAccess64;
		DIAGNOSTICS_API static dbgHelpSymGetLineFromAddr64Function symGetLineFromAddr64;
		DIAGNOSTICS_API static dbgHelpGetSymbolFromAddressFunction symGetSymFromAddr64;
		DIAGNOSTICS_API static dbgHelpSymGetModuleBase64Function symGetModuleBase64;
		DIAGNOSTICS_API static dbgHelpSymGetModuleInfo64Function symGetModuleInfo64;
		DIAGNOSTICS_API static dbgHelpSymLoadModule64Function symLoadModule64;
		DIAGNOSTICS_API static kernel32CreateToolhelp32SnapshotFunction createToolhelp32Snapshot;
		DIAGNOSTICS_API static kernel32Module32FirstFunction module32First;
		DIAGNOSTICS_API static kernel32Module32NextFunction module32Next;
		DIAGNOSTICS_API static kernel32CloseHandleFunction closeHandle;
		DIAGNOSTICS_API static kernel32CancelIoExFunction cancelIoEx;
	};
}
