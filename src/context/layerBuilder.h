#pragma once
#include <phi.h>
#include "contextApi.h"

#include <rendering\gl.h>
#include <rendering\program.h>

#include "layer.h"
#include <stack>
namespace phi
{
    class layerBuilder
    {
    private:
        layer* _layer;
        resolution _resolution;
        string _resourcesPath;

        vector<renderPass*> _meshRenderPasses;
        vector<renderPass*> _controlRenderPasses;
        vector<renderPass*> _glassyControlRenderPasses;
        vector<renderPass*> _textRenderPasses;

    private:
        layerBuilder(layer* layer, resolution resolution, string resourcesPath);

    public:
        CONTEXT_API static layerBuilder newLayer(camera* camera, string resourcesPath);
        CONTEXT_API layerBuilder withMeshRenderer();
        CONTEXT_API layerBuilder withControlRenderer();
        CONTEXT_API layerBuilder withGlassyControlRenderer();
        CONTEXT_API layerBuilder withTextRenderer();
        CONTEXT_API layer* build();
    };
}