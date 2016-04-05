#pragma once
#include "diagnosticsApi.h"
#include <phi.h>

namespace phi
{
    struct symbolModule
    {
        symbolModule()
        {
        }

        symbolModule(char * name, char * path) :
            name(name),
            path(path)
        {
        }

        char* name;
        char* path;
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
        char* name;
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

        char* name;
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
