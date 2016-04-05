#pragma once
#include <phi.h>
#include "renderingApi.h"

namespace phi
{
    class glError
    {
    private:
        static string getErrorString(GLenum error);

    public:
        RENDERING_API static void check();
    };
}