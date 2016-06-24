#pragma once
#include <phi.h>
#include "contextApi.h"

#include <rendering\program.h>
#include <rendering\framebufferAllocator.h>

#include <application\commandsManager.h>

#include "layer.h"

namespace phi
{
    class layerBuilder
    {
    private:
        layer* _layer;
        resolution _resolution;
        string _resourcesPath;
        framebufferAllocator* _framebufferAllocator;
        commandsManager* _commandsManager;

        vector<renderPass*> _meshRenderPasses;
        vector<renderPass*> _controlRenderPasses;
        vector<renderPass*> _glassyControlRenderPasses;
        vector<renderPass*> _textRenderPasses;

    private:
        layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);

    public:
        CONTEXT_API static layerBuilder newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);
        CONTEXT_API layerBuilder withMeshRenderer();
        CONTEXT_API layerBuilder withControlRenderer();
        CONTEXT_API layerBuilder withGlassyControlRenderer();
        CONTEXT_API layerBuilder withTextRenderer();
        CONTEXT_API layer* build();
    };
}