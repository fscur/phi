#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "shader.h"

namespace phi
{
    class liveShaderReloader
    {
    public:
        RENDERING_API static map<string, shader*> shaders;

    public:
        RENDERING_API static void reloadShader(string name);
    };
}