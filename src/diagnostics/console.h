#pragma once
#include "diagnostics.h"
#include <string>
namespace phi
{
    class console
    {
    public:
        console() { };
        ~console() { };
        static void write(std::string message);
        static void writeLine(std::string message);
    };
}