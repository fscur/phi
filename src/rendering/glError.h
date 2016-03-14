#pragma once
#include <phi.h>
#include "rendering.h"

namespace phi
{
    class glError
    {
    private:
        static string getErrorString(GLenum error);
        static void printError(string msg);

    public:
        RENDERING_API static void check();
    };
}