#pragma once
#include <phi.h>
#include "contextApi.h"

#include <rendering\program.h>
#include <rendering\framebufferAllocator.h>

#include <application\commandsManager.h>

#include "layer.h"
#include "meshLayerBehaviour.h"
#include "physicsLayerBehaviour.h"
#include "nodeTranslator.h"

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
        meshLayerBehaviour* _meshBehaviour;
        physicsLayerBehaviour* _physicsBehaviour;
        
        vector<renderPass*> _meshRenderPasses;
        vector<renderPass*> _controlRenderPasses;
        vector<renderPass*> _glassyControlRenderPasses;
        vector<renderPass*> _textRenderPasses;

        bool _withMeshRenderer;
        bool _withBoxColliderRenderer;
        bool _withPlaneGridRenderer;
        bool _withControlRenderer;
        bool _withGlassyControlRenderer;
        bool _withTextRenderer;
        bool _withPhysics;
        bool _withAnimation;
        bool _withCameraController;
        bool _withSelectionController;
        bool _withObbTranslationController;

    private:
        layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);

        void buildMeshRenderer();
        void buildBoxColliderRenderer();
        void buildPlaneGridRenderer();
        void buildControlRenderer();
        void buildGlassyControlRenderer();
        void buildTextRenderer();
        void buildPhysics();
        void buildAnimation();
        void buildCameraController();
        void buildSelectionController(meshLayerBehaviour* meshBehaviour);
        void buildObbTranslationController();

    public:
        CONTEXT_API static layerBuilder newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);
        layerBuilder withMeshRenderer() { _withMeshRenderer = true; return *this; }
        layerBuilder withBoxColliderRenderer() { _withBoxColliderRenderer = true; return *this; }
        layerBuilder withPlaneGridRenderer() { _withPlaneGridRenderer = true; return *this; }
        layerBuilder withControlRenderer() { _withControlRenderer = true; return *this; }
        layerBuilder withGlassyControlRenderer() { _withGlassyControlRenderer = true; return *this; }
        layerBuilder withTextRenderer() { _withTextRenderer = true; return *this; }
        layerBuilder withPhysics() { _withPhysics = true; return *this; }
        layerBuilder withAnimation() { _withAnimation = true; return *this; }
        layerBuilder withCameraController() { _withCameraController = true; return *this; }
        layerBuilder withSelectionController() { _withSelectionController = true; return *this; }
        layerBuilder withObbTranslationController() { _withObbTranslationController = true; return *this; }

        CONTEXT_API layer* build();
    };
}
