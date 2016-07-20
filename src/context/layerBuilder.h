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

        bool _withMeshRenderer;
        bool _withDebugRenderer;
        bool _withControlRenderer;
        bool _withGlassyControlRenderer;
        bool _withTextRenderer;

    private:
        layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);

        void buildMeshRenderer();
        void buildDebugRenderer();
        void buildControlRenderer();
        void buildGlassyControlRenderer();
        void buildTextRenderer();

    public:
        CONTEXT_API static layerBuilder newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);
        layerBuilder withMeshRenderer() { _withMeshRenderer = true; return *this; }
        layerBuilder withDebugRenderer() { _withDebugRenderer = true; return *this; }
        layerBuilder withControlRenderer() { _withControlRenderer = true; return *this; }
        layerBuilder withGlassyControlRenderer() { _withGlassyControlRenderer = true; return *this; }
        layerBuilder withTextRenderer() { _withTextRenderer = true; return *this; }

        CONTEXT_API layer* build();
    };
}