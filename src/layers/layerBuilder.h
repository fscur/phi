#pragma once
#include <phi.h>
#include "layersApi.h"

#include <animation\animatorLayerBehaviour.h>
#include <application\commandsManager.h>

#include <context\invalidLayerConfigurationException.h>
#include <context\layer.h>
#include <physics\physicsLayerBehaviour.h>

#include <rendering\camera.h>
#include <rendering\framebufferAllocator.h>
#include <rendering\program.h>

#include <debugRendering\boxColliderLayerBehaviour.h>
#include <sceneRendering\ghostMeshLayerBehaviour.h>
#include <sceneRendering\meshLayerBehaviour.h>
#include <sceneRendering\meshRenderer.h>
#include <sceneRendering\planeGridLayerBehaviour.h>

#include <uiRendering\controlLayerBehaviour.h>
#include <uiRendering\controlRenderer.h>
#include <uiRendering\glassyControlLayerBehaviour.h>
#include <uiRendering\glassyControlRenderer.h>
#include <uiRendering\textLayerBehaviour.h>
#include <uiRendering\textRenderer.h>

#include "nodeSelection\selectionInputController.h"
#include "cameraControl\cameraInputController.h"
#include "nodeTranslation\translationInputController.h"

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
        bool _withBoxColliderRenderer;
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
        void buildBoxColliderRenderer();
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
        LAYERS_API static layerBuilder newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);
        layerBuilder withMeshRenderer() { _withMeshRenderer = true; return *this; }
        layerBuilder withGhostMeshRenderer() { _withGhostMeshRenderer = true; return *this; }
        layerBuilder withBoxColliderRenderer() { _withBoxColliderRenderer = true; return *this; }
        layerBuilder withPlaneGridRenderer() { _withPlaneGridRenderer = true; return *this; }
        layerBuilder withControlRenderer() { _withControlRenderer = true; return *this; }
        layerBuilder withGlassyControlRenderer() { _withGlassyControlRenderer = true; return *this; }
        layerBuilder withTextRenderer() { _withTextRenderer = true; return *this; }
        layerBuilder withPhysics() { _withPhysics = true; return *this; }
        layerBuilder withAnimation() { _withAnimation = true; return *this; }
        layerBuilder withCameraController() { _withCameraController = true; return *this; }
        layerBuilder withSelectionController() { _withSelectionController = true; return *this; }
        layerBuilder withTranslationController() { _withTranslationController = true; return *this; }

        LAYERS_API layer* build();
    };
}
