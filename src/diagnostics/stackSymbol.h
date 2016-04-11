#pragma once
#include "diagnosticsApi.h"
#include <phi.h>

namespace phi
{
    struct symbolModule
    {
        char* name;
        char* path;

        symbolModule() :
            name((char*)("")),
            path((char*)(""))
        {
        }

        symbolModule(char* name, char* path) :
            name(name),
            path(path)
        {
        }
    };

    struct symbolFile
    {
        char* name;
        unsigned int line;

        symbolFile() :
            name((char*)("")),
            line(0)
        {
        }

        symbolFile(char* name, unsigned int line) :
            name(name),
            line(line)
        {
        }
    };

    struct stackSymbol
    {
        char* name;
        uintptr_t address;
        symbolFile file;
        symbolModule module;

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

        string toString()
        {
            auto lineNumber = file.line != 0 ? "(" + std::to_string(file.line) + "): " : "";
            auto separator = strcmp(module.name, "") == 0 ? "" : "::";

            std::stringstream stream;
            stream << std::hex << address << " " << file.name << lineNumber << module.name << separator << name;

            return stream.str();
        }
    };
}
