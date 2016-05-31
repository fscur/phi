#include <precompiled.h>
#include "stackTracer.h"
#include "win64ProcLibrary.h"
#include "fileUtils.h"


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
        win64DataDefinitions::line64 line = {};
        line.sizeOfStruct = sizeof(win64DataDefinitions::line64);

        if (win64ProcLibrary::symGetLineFromAddr64(process, symbolAddress, &displacement, &line))
        {
            auto fileNameSize = strlen(line.fileName) + 1;
            auto symbolFile = phi::symbolFile();

            symbolFile.name = static_cast<char*>(malloc(sizeof(char) * fileNameSize));
            symbolFile.line = line.lineNumber;

            strcpy_s(symbolFile.name, fileNameSize, line.fileName);

            return symbolFile;
        }
        else
        {
            return symbolFile();
        }
    }

    symbolModule stackTracer::symbolModuleFromAddress(HANDLE process, uintptr_t symbolAddress)
    {
        win64DataDefinitions::module64 module = {};
        module.sizeOfStruct = sizeof(win64DataDefinitions::module64);

        if (win64ProcLibrary::symGetModuleInfo64(process, symbolAddress, &module))
        {
            auto moduleNameSize = strlen(module.moduleName) + 1;
            auto modulePathSize = strlen(module.loadedImageName) + 1;
            auto symbolModule = phi::symbolModule();

            symbolModule.name = static_cast<char*>(malloc(sizeof(char) * moduleNameSize));
            symbolModule.path = static_cast<char*>(malloc(sizeof(char) * modulePathSize));

            strcpy_s(symbolModule.name, moduleNameSize, module.moduleName);
            strcpy_s(symbolModule.path, modulePathSize, module.loadedImageName);

            return symbolModule;
        }
        else
        {
            return symbolModule();
        }
    }

    void stackTracer::takeSnapshot(HANDLE process)
    {
        auto snapshot = win64ProcLibrary::createToolhelp32Snapshot(win64DataDefinitions::SNAPMODULE, GetCurrentProcessId());

        win64DataDefinitions::moduleEntry32 moduleEntry;
        moduleEntry.dwSize = sizeof(win64DataDefinitions::moduleEntry32);

        auto hasModules = win64ProcLibrary::module32First(snapshot, &moduleEntry);
        while (hasModules)
        {
            win64ProcLibrary::symLoadModule64(
                process, 0,
                (PSTR)moduleEntry.szExePath,
                (PSTR)moduleEntry.szModule,
                (DWORD64)moduleEntry.modBaseAddr,
                moduleEntry.modBaseSize);

            hasModules = win64ProcLibrary::module32Next(snapshot, &moduleEntry);
        }

        CloseHandle(snapshot);
    }

    DWORD stackTracer::getPcImageType()
    {
        return IMAGE_FILE_MACHINE_AMD64; //TODO: check all pc image types
    }

    win64DataDefinitions::stackFrame stackTracer::buildStackFrame(CONTEXT context)
    {
        win64DataDefinitions::stackFrame stackFrame = {};
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrPC.Mode = win64DataDefinitions::addressMode::AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Rsp;
        stackFrame.AddrFrame.Mode = win64DataDefinitions::addressMode::AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Rsp;
        stackFrame.AddrStack.Mode = win64DataDefinitions::addressMode::AddrModeFlat;

        return stackFrame;
    }

    win64DataDefinitions::symbol64* stackTracer::buildSymbolPointer()
    {
        auto symbolPointer = (win64DataDefinitions::symbol64*)malloc(sizeof(win64DataDefinitions::symbol64) + MAX_SYMBOL_NAME_LENGTH);
        memset(symbolPointer, 0, sizeof(win64DataDefinitions::symbol64) + MAX_SYMBOL_NAME_LENGTH);
        symbolPointer->sizeOfStruct = sizeof(win64DataDefinitions::symbol64);
        symbolPointer->maxNameLength = MAX_SYMBOL_NAME_LENGTH;

        return symbolPointer;
    }

    stackSymbol stackTracer::buildStackSymbolFromAddress(HANDLE process, uintptr_t callAddress)
    {
        auto symbol = buildSymbolPointer();
        win64ProcLibrary::symGetSymFromAddr64(process, callAddress, 0, symbol);

        auto symbolNameSize = strlen(symbol->name) + 1;
        auto stackSymbol = phi::stackSymbol();

        stackSymbol.name = static_cast<char*>(malloc(sizeof(char) * symbolNameSize));
        strcpy_s(stackSymbol.name, symbolNameSize, symbol->name);

        stackSymbol.file = symbolFileFromAddress(process, callAddress);
        stackSymbol.module = symbolModuleFromAddress(process, callAddress);
        stackSymbol.address = callAddress;

        free(symbol);
        return stackSymbol;
    }

    vector<stackSymbol> stackTracer::captureStackBackTrace()
    {
        const auto maxCallsToCapture = 1000;
        void* stackCalls[maxCallsToCapture];
        auto process = GetCurrentProcess();
        auto callsCount = CaptureStackBackTrace(0, maxCallsToCapture, stackCalls, NULL);

        win64ProcLibrary::symInitialize(process, NULL, TRUE);
        takeSnapshot(process);

        auto stack = vector<stackSymbol>();

        for (auto i = 0; i < callsCount; i++)
        {
            auto callAddress = reinterpret_cast<uintptr_t>(stackCalls[i]);
            auto stackSymbol = buildStackSymbolFromAddress(process, callAddress);

            stack.push_back(stackSymbol);
        }

        return stack;
    }

    vector<stackSymbol> stackTracer::stackWalk64()
    {
        auto thread = GetCurrentThread();
        auto process = GetCurrentProcess();

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        win64ProcLibrary::symInitialize(process, NULL, FALSE);
        takeSnapshot(process);

        auto imageType = getPcImageType();
        auto stackFrame = buildStackFrame(context);
        auto stack = vector<stackSymbol>();
        auto framesToSkip = 1;
        while (win64ProcLibrary::stackWalk64(
            imageType,
            process,
            thread,
            &stackFrame,
            &context,
            NULL,
            win64ProcLibrary::symFunctionTableAccess64,
            win64ProcLibrary::symGetModuleBase64,
            NULL))
        {
            if (framesToSkip > 0)
            {
                --framesToSkip;
                continue;
            }

            auto callAddress = static_cast<uintptr_t>(stackFrame.AddrPC.Offset);
            auto returnAddress = static_cast<uintptr_t>(stackFrame.AddrReturn.Offset);

            if (callAddress == returnAddress || callAddress == 0)
                break;

            auto stackSymbol = buildStackSymbolFromAddress(process, callAddress);
            stack.push_back(stackSymbol);
        }

        return stack;
    }

    stackSymbol stackTracer::captureAllocationSight()
    {
        auto thread = GetCurrentThread();
        auto process = GetCurrentProcess();

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        win64ProcLibrary::symInitialize(process, NULL, FALSE);
        takeSnapshot(process);

        auto imageType = getPcImageType();
        auto stackFrame = buildStackFrame(context);
        auto symbol = buildSymbolPointer();
        auto stackSymbol = phi::stackSymbol();

        char const* cppExtension = ".cpp";
        char const* headerExtension = ".h";

        int framesToSkip = 3;
        while (win64ProcLibrary::stackWalk64(
            imageType,
            process,
            thread,
            &stackFrame,
            &context,
            NULL,
            win64ProcLibrary::symFunctionTableAccess64,
            win64ProcLibrary::symGetModuleBase64,
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
                win64ProcLibrary::symGetSymFromAddr64(process, symbolAddress, 0, symbol);
                stackSymbol.name = _strdup(symbol->name);
                stackSymbol.address = symbolAddress;
                stackSymbol.file.line = file.line;
                stackSymbol.file.name = _strdup(file.name);

                break;
            }
        }

        free(symbol);
        return stackSymbol;
    }

    stackSymbol stackTracer::optimizedCaptureAllocationSight()
    {
        auto thread = GetCurrentThread();
        auto process = GetCurrentProcess();

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        win64ProcLibrary::symInitialize(process, nullptr, false);

        auto snapshot = win64ProcLibrary::createToolhelp32Snapshot(win64DataDefinitions::SNAPMODULE, GetCurrentProcessId());
        win64DataDefinitions::moduleEntry32 moduleEntry;
        moduleEntry.dwSize = sizeof(win64DataDefinitions::moduleEntry32);

        auto hasModules = win64ProcLibrary::module32First(snapshot, &moduleEntry);
        while (hasModules)
        {
            win64ProcLibrary::symLoadModule64(
                process, 0,
                (PSTR)moduleEntry.szExePath,
                (PSTR)moduleEntry.szModule,
                (DWORD64)moduleEntry.modBaseAddr,
                moduleEntry.modBaseSize);

            hasModules = win64ProcLibrary::module32Next(snapshot, &moduleEntry);
        }

        win64ProcLibrary::closeHandle(snapshot);

        auto imageType = IMAGE_FILE_MACHINE_AMD64;

        win64DataDefinitions::stackFrame stackFrame = {};
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrPC.Mode = win64DataDefinitions::addressMode::AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Rsp;
        stackFrame.AddrFrame.Mode = win64DataDefinitions::addressMode::AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Rsp;
        stackFrame.AddrStack.Mode = win64DataDefinitions::addressMode::AddrModeFlat;

        auto symbolPointer = (win64DataDefinitions::symbol64*)malloc(sizeof(win64DataDefinitions::symbol64) + MAX_SYMBOL_NAME_LENGTH);
        memset(symbolPointer, 0, sizeof(win64DataDefinitions::symbol64) + MAX_SYMBOL_NAME_LENGTH);
        symbolPointer->sizeOfStruct = sizeof(win64DataDefinitions::symbol64);
        symbolPointer->maxNameLength = MAX_SYMBOL_NAME_LENGTH;

        auto stackSymbol = phi::stackSymbol();

        char const* cppExtension = ".cpp";
        char const* headerExtension = ".h";

        int framesToSkip = 3;
        while (win64ProcLibrary::stackWalk64(
            imageType,
            process,
            thread,
            &stackFrame,
            &context,
            NULL,
            win64ProcLibrary::symFunctionTableAccess64,
            win64ProcLibrary::symGetModuleBase64,
            NULL))
        {
            if (framesToSkip > 0)
            {
                --framesToSkip;
                continue;
            }

            auto symbolAddress = (uintptr_t)stackFrame.AddrPC.Offset;
            auto returnAddress = (uintptr_t)stackFrame.AddrReturn.Offset;

            if (symbolAddress == returnAddress || symbolAddress == 0)
                break;

            DWORD displacement;
            win64DataDefinitions::line64 line = {};
            line.sizeOfStruct = sizeof(win64DataDefinitions::line64);

            if (win64ProcLibrary::symGetLineFromAddr64(process, symbolAddress, &displacement, &line))
            {
                auto endOfFileName = line.fileName + strlen(line.fileName);

                if (strcmp(endOfFileName - 4, cppExtension) == 0 ||
                    strcmp(endOfFileName - 2, headerExtension) == 0)
                {
                    win64ProcLibrary::symGetSymFromAddr64(process, symbolAddress, 0, symbolPointer);
                    stackSymbol.name = _strdup(symbolPointer->name);
                    stackSymbol.address = symbolAddress;
                    stackSymbol.file.line = line.lineNumber;
                    stackSymbol.file.name = _strdup(line.fileName);

                    break;
                }
            }
        }

        free(symbolPointer);
        return stackSymbol;
    }

    void stackTracer::printStackTrace(vector<stackSymbol> stack)
    {
        OutputDebugString("\n\nstack trace:\n");
        char messageBuffer[4096];
        for (auto& symbol : stack)
        {
            sprintf_s(messageBuffer, "%s (%d): at %s::%s\n", symbol.file.name, symbol.file.line, symbol.module.name, symbol.name);
            OutputDebugString(messageBuffer);
        }

        OutputDebugString("\n\n");
    }
}
