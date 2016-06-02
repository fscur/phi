#pragma once
#include <phi.h>
#include "contextApi.h"

#include <rendering\gl.h>
#include <rendering\program.h>

#include "layer.h"

namespace phi
{
    class layerBuilder
    {
    private:
        static program* buildProgram(
            const string& shadersPath, 
            const string& programName,
            const string& vertexShaderName, 
            const string& fragmentShaderName);

        static map<string, shader*> _shadersCache;
        static map<std::tuple<shader*, shader*>, program*> _programsCache;

    public:
        CONTEXT_API static layer* buildScene(const string& resourcesPath, gl* gl, float width, float height);
        CONTEXT_API static layer* buildUI(const string& resourcesPath, gl* gl, float width, float height);
    };
}