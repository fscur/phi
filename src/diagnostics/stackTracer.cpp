#include <precompiled.h>
#include "stackTracer.h"

namespace phi
{
    stackTracer::stackTracer()
    {
    }

    stackTracer::~stackTracer()
    {
    }

    symbolFile stackTracer::symbolFileFromAddress(HANDLE process, uintptr_t symbolAddress)
    {
        DWORD displacement;
        IMAGEHLP_LINE64 line = {};
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (SymGetLineFromAddr64(process, symbolAddress, &displacement, &line))
        {
            return symbolFile(line.FileName, line.LineNumber);
        }
        else
        {
            return symbolFile("", 0);
        }
    }

    symbolModule stackTracer::symbolModuleFromAddress(HANDLE process, uintptr_t symbolAddress)
    {
        IMAGEHLP_MODULE64 module = {};
        module.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

        if (SymGetModuleInfo64(process, symbolAddress, &module))
        {
            return symbolModule(module.ModuleName, module.LoadedImageName);
        }
        else
        {
            return symbolModule("", "");
        }
    }

    void stackTracer::takeSnapshot(HANDLE process)
    {
        auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);

        auto hasModules = Module32First(snapshot, &moduleEntry);
        while (hasModules)
        {
            SymLoadModule64(
                process, 0,
                (PSTR)moduleEntry.szExePath,
                (PSTR)moduleEntry.szModule,
                (DWORD64)moduleEntry.modBaseAddr,
                moduleEntry.modBaseSize);

            hasModules = Module32Next(snapshot, &moduleEntry);
        }

        CloseHandle(snapshot);
    }

    DWORD stackTracer::getPcImageType()
    {
        return IMAGE_FILE_MACHINE_AMD64; //TODO: check all pc image types
    }

    STACKFRAME64 stackTracer::buildStackFrame(CONTEXT context)
    {
        STACKFRAME64 stackFrame = {};
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Rsp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Rsp;
        stackFrame.AddrStack.Mode = AddrModeFlat;

        return stackFrame;
    }

    IMAGEHLP_SYMBOL64* stackTracer::buildSymbol64Pointer()
    {
        auto symbolPointer = (IMAGEHLP_SYMBOL64*)malloc(sizeof(IMAGEHLP_SYMBOL64) + MAX_SYMBOL_NAME_LENGTH);
        memset(symbolPointer, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_SYMBOL_NAME_LENGTH);
        symbolPointer->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        symbolPointer->MaxNameLength = MAX_SYMBOL_NAME_LENGTH;

        return symbolPointer;
    }

    SYMBOL_INFO* stackTracer::buildSymbolPointer()
    {
        auto symbolPointer = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME_LENGTH);
        memset(symbolPointer, 0, sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME_LENGTH);
        symbolPointer->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbolPointer->MaxNameLen = MAX_SYMBOL_NAME_LENGTH;

        return symbolPointer;
    }

    vector<stackSymbol> stackTracer::captureStackBackTrace()
    {
        const auto maxCallsToCapture = 1000;
        void* stackCalls[maxCallsToCapture];
        auto process = GetCurrentProcess();
        auto callsCount = CaptureStackBackTrace(0, maxCallsToCapture, stackCalls, NULL);

        SymInitialize(process, NULL, TRUE);
        takeSnapshot(process);

        auto symbol = buildSymbol64Pointer();
        auto stack = vector<stackSymbol>();

        for (auto i = 0; i < callsCount; i++)
        {
            auto callAddress = (DWORD64)(stackCalls[i]);
            SymGetSymFromAddr64(process, callAddress, 0, symbol);
            //SymFromAddr(process, callAddress, 0, symbol);

            auto address = static_cast<uintptr_t>(symbol->Address);
            auto module = symbolModuleFromAddress(process, address);
            auto file = symbolFileFromAddress(process, address);

            stack.push_back(stackSymbol(symbol->Name, address, file, module));
        }

        free(symbol);
        return stack;
    }

    vector<stackSymbol> stackTracer::stackWalk64()
    {
        auto thread = GetCurrentThread();
        auto process = GetCurrentProcess();

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        SymInitialize(process, NULL, FALSE);
        takeSnapshot(process);

        auto imageType = getPcImageType();
        auto stackFrame = buildStackFrame(context);
        auto symbol = buildSymbol64Pointer();
        auto stack = vector<stackSymbol>();

        while (StackWalk64(
            imageType,
            process,
            thread,
            &stackFrame,
            &context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL))
        {
            auto symbolAddress = static_cast<uintptr_t>(stackFrame.AddrPC.Offset);
            auto returnAddress = static_cast<uintptr_t>(stackFrame.AddrReturn.Offset);

            if (symbolAddress == returnAddress || symbolAddress == 0)
                break;

            SymGetSymFromAddr64(process, symbolAddress, 0, symbol);

            auto module = symbolModuleFromAddress(process, symbolAddress);
            auto file = symbolFileFromAddress(process, symbolAddress);

            stack.push_back(stackSymbol(symbol->Name, symbolAddress, file, module));
        }

        free(symbol);
        return stack;
    }

    stackSymbol stackTracer::captureAllocationSight()
    {
        auto thread = GetCurrentThread();
        auto process = GetCurrentProcess();

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        SymInitialize(process, NULL, FALSE);
        takeSnapshot(process);

        auto imageType = getPcImageType();
        auto stackFrame = buildStackFrame(context);
        auto symbol = buildSymbolPointer();
        auto stackSymbol = phi::stackSymbol();

        char* cppExtension = ".cpp";
        char* headerExtension = ".h";

        int framesToSkip = 3;
        while (StackWalk64(
            imageType,
            process,
            thread,
            &stackFrame,
            &context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL))
        {
            if (framesToSkip > 0)
            {
                --framesToSkip;
                continue;
            }

            auto symbolAddress = static_cast<uintptr_t>(stackFrame.AddrPC.Offset);
            auto returnAddress = static_cast<uintptr_t>(stackFrame.AddrReturn.Offset);

            if (symbolAddress == returnAddress || symbolAddress == 0)
                break;

            auto file = symbolFileFromAddress(process, symbolAddress);
            auto endOfFileName = file.name + strlen(file.name);

            if (strcmp(endOfFileName - 4, cppExtension) == 0 ||
                strcmp(endOfFileName - 2, headerExtension) == 0)
            {
                //TODO: choose best SymFromAddr
                SymFromAddr(process, symbolAddress, 0, symbol);
                stackSymbol.name = _strdup(symbol->Name);
                stackSymbol.address = symbolAddress;
                stackSymbol.file.line = file.line;
                stackSymbol.file.name = _strdup(file.name);

                break;
            }
        }

        free(symbol);
        return stackSymbol;
    }

    void stackTracer::printStackTrace(vector<stackSymbol> stack)
    {
        std::cout << "Printing stack" << std::endl;

        for (auto& symbol : stack)
        {
            std::cout << symbol.toString() << std::endl;
            //printf("0x%08X %s(%i): %s\n", symbol.address, symbol.file.name.c_str(), symbol.file.line, symbol.name.c_str());
        }

        std::cout << std::endl;
    }
}
