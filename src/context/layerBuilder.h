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
            const string& vertexShaderName, 
            const string& fragmentShaderName);

        static map<string, shader*> _shadersCache;
        static map<std::tuple<shader*, shader*>, program*> _programsCache;

        layer* _layer;
        gl* _gl;
        float _width;
        float _height;
        string _resourcesPath;

    private:
        layerBuilder(layer* layer, gl* gl, float width, float height, string resourcesPath);

    public:
        CONTEXT_API ~layerBuilder();
        CONTEXT_API static layerBuilder newLayer(camera* camera, gl* gl, float width, float height, string resourcesPath);
        CONTEXT_API layerBuilder withMeshRenderer();
        CONTEXT_API layerBuilder withControlRenderer();
        CONTEXT_API layerBuilder withTextRenderer();
        CONTEXT_API layer* build();
    };
}