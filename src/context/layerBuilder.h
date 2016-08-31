#pragma once
#include <phi.h>
#include "contextApi.h"

#include <rendering\program.h>
#include <rendering\framebufferAllocator.h>

#include <application\commandsManager.h>

#include "layer.h"
#include "meshLayerBehaviour.h"
#include "ghostMeshLayerBehaviour.h"
#include "physicsLayerBehaviour.h"
#include "selectionInputController.h"

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
        ghostMeshLayerBehaviour* _ghostMeshBehaviour;
        physicsLayerBehaviour* _physicsBehaviour;
        selectionInputController* _selectionInputController;

        vector<renderPass*> _meshRenderPasses;
        vector<renderPass*> _controlRenderPasses;
        vector<renderPass*> _glassyControlRenderPasses;
        vector<renderPass*> _textRenderPasses;

        bool _withMeshRenderer;
        bool _withGhostMeshRenderer;
        bool _withObbRenderer;
        bool _withPlaneGridRenderer;
        bool _withControlRenderer;
        bool _withGlassyControlRenderer;
        bool _withTextRenderer;
        bool _withPhysics;
        bool _withAnimation;
        bool _withCameraController;
        bool _withSelectionController;
        bool _withTranslationController;

    private:
        layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);

        void buildMeshRenderer();
        void buildGhostMeshRenderer();
        void buildObbRenderer();
        void buildPlaneGridRenderer();
        void buildControlRenderer();
        void buildGlassyControlRenderer();
        void buildTextRenderer();
        void buildPhysics();
        void buildAnimation();
        void buildCameraController();
        void buildSelectionController();
        void buildTranslationController();

    public:
        CONTEXT_API static layerBuilder newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);
        layerBuilder withMeshRenderer() { _withMeshRenderer = true; return *this; }
        layerBuilder withGhostMeshRenderer() { _withGhostMeshRenderer = true; return *this; }
        layerBuilder withObbRenderer() { _withObbRenderer = true; return *this; }
        layerBuilder withPlaneGridRenderer() { _withPlaneGridRenderer = true; return *this; }
        layerBuilder withControlRenderer() { _withControlRenderer = true; return *this; }
        layerBuilder withGlassyControlRenderer() { _withGlassyControlRenderer = true; return *this; }
        layerBuilder withTextRenderer() { _withTextRenderer = true; return *this; }
        layerBuilder withPhysics() { _withPhysics = true; return *this; }
        layerBuilder withAnimation() { _withAnimation = true; return *this; }
        layerBuilder withCameraController() { _withCameraController = true; return *this; }
        layerBuilder withSelectionController() { _withSelectionController = true; return *this; }
        layerBuilder withTranslationController() { _withTranslationController = true; return *this; }

        CONTEXT_API layer* build();
    };
}
