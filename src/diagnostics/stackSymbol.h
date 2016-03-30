#pragma once
#include "diagnostics.h"
#include <phi.h>

namespace phi
{
    struct symbolModule
    {
        symbolModule()
        {
        }

        symbolModule(char* name, char* path) :
            name(name),
            path(path)
        {
        }

        const char* name;
        const char* path;
    };

    struct symbolFile
    {
        symbolFile()
        {
        }

        symbolFile(char* name, unsigned int line) :
            name(name),
            line(line)
        {
        }

        unsigned int line;
        const char* name;
    };

    struct stackSymbol
    {
        stackSymbol()
        {
        }

        stackSymbol(char* name, uintptr_t address, symbolFile file, symbolModule module) :
            name(name),
            address(address),
            file(file),
            module(module)
        {
        }

        const char* name;
        uintptr_t address;
        symbolFile file;
        symbolModule module;

        string toString()
        {
            auto lineNumber = file.line != 0 ? "(" + std::to_string(file.line) + "): " : "";
            auto separator = module.name!= "" ? ":: " : "";

            std::stringstream stream;
            stream << std::hex << address << " " << file.name << lineNumber << module.name << separator << name;

            return stream.str();
        }
    };
}
