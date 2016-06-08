#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "program.h"

namespace phi
{
    class programBuilder
    {
    private:
        static map<string, shader*> _shadersCache;

    private:
        ~programBuilder() {};

    public:
        RENDERING_API static program* buildProgram(
            const string& shadersPath,
            const string& vertexShaderName,
            const string& fragmentShaderName);
    };
}