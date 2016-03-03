#pragma once
#include <phi.h>
#include "diagnostics.h"

namespace phi
{
    class console
    {
    public:
        console() { };
        ~console() { };
        static void write(string message);
        static void writeLine(string message);
    };
}