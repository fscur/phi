#pragma once
#include <phi.h>
#include "diagnosticsApi.h"

namespace phi
{
	class platformProcLoader
	{
	private:
		static bool _procsAreLoaded;
#ifdef _WIN32
		static const char* _dbgHelpLibraryName;
		static const char* _kernelLibraryName;
		static void loadDbgHelpProcs(const char* executableFileName);
		static void loadKernel32Procs(const char *executableFileName);
#endif
	public:
		DIAGNOSTICS_API static void load(const char* executableFileName);
	};
}