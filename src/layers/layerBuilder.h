#pragma once
#include <phi.h>

#include <animation/animatorLayerBehaviour.h>
#include <application/commandsManager.h>

#include <context/invalidLayerConfigurationException.h>
#include <context/layer.h>

#include <physics/physicsLayerBehaviour.h>

#include <rendering/camera.h>
#include <rendering/framebufferAllocator.h>
#include <rendering/program.h>

#include <debugRendering/obbLayerBehaviour.h>
#include <debugRendering/boxColliderLayerBehaviour.h>

#include <sceneRendering/ghostMeshLayerBehaviour.h>
#include <sceneRendering/meshLayerBehaviour.h>
#include <sceneRendering/meshRenderer.h>
#include <sceneRendering/planeGridLayerBehaviour.h>

#include <uiRendering/controlLayerBehaviour.h>
#include <uiRendering/controlRenderer.h>
#include <uiRendering/glassyControlLayerBehaviour.h>
#include <uiRendering/glassyControlRenderer.h>
#include <uiRendering/textLayerBehaviour.h>
#include <uiRendering/textRenderer.h>

#include "layersApi.h"
#include "nodeSelection/selectionInputController.h"
#include "nodeSelection/selectionLayerBehaviour.h"
#include "cameraControl/cameraInputController.h"
#include "nodeTranslation/translationInputController.h"
#include "nodeRotation/rotationInputController.h"

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
        selectionLayerBehaviour* _selectionBehaviour;

        vector<renderPass*> _meshRenderPasses;
        vector<renderPass*> _controlRenderPasses;
        vector<renderPass*> _glassyControlRenderPasses;
        vector<renderPass*> _textRenderPasses;

        bool _withMeshRenderer;
        bool _withGhostMeshRenderer;
        bool _withObbRenderer;
        bool _withBoxColliderRenderer;
        bool _withPlaneGridRenderer;
        bool _withRotationPlaneGridRenderer;
        bool _withControlRenderer;
        bool _withGlassyControlRenderer;
        bool _withTextRenderer;
        bool _withPhysics;
        bool _withAnimation;
        bool _withCameraController;
        bool _withSelectionController;
        bool _withTranslationController;
        bool _withRotationController;
        bool _withUIMouseController;

    private:
        layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);

        void buildMeshRenderer();
        void buildGhostMeshRenderer();
        void buildObbRenderer();
        void buildBoxColliderRenderer();
        void buildPlaneGridRenderer();
        void buildRotationPlaneGridRenderer();
        void buildControlRenderer();
        void buildGlassyControlRenderer();
        void buildTextRenderer();
        void buildPhysics();
        void buildAnimation();
        void buildCameraController();
        void buildSelectionController();
        void buildTranslationController();
        void buildRotationController();
        void buildUIMouseController();

    public:
        LAYERS_API static layerBuilder newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager);
        layerBuilder withMeshRenderer() { _withMeshRenderer = true; return *this; }
        layerBuilder withGhostMeshRenderer() { _withGhostMeshRenderer = true; return *this; }
        layerBuilder withObbRenderer() { _withObbRenderer = true; return *this; }
        layerBuilder withBoxColliderRenderer() { _withBoxColliderRenderer = true; return *this; }
        layerBuilder withPlaneGridRenderer() { _withPlaneGridRenderer = true; return *this; }
        layerBuilder withRotationPlaneGridRenderer() { _withRotationPlaneGridRenderer = true; return *this; }
        layerBuilder withControlRenderer() { _withControlRenderer = true; return *this; }
        layerBuilder withGlassyControlRenderer() { _withGlassyControlRenderer = true; return *this; }
        layerBuilder withTextRenderer() { _withTextRenderer = true; return *this; }
        layerBuilder withPhysics() { _withPhysics = true; return *this; }
        layerBuilder withAnimation() { _withAnimation = true; return *this; }
        layerBuilder withCameraController() { _withCameraController = true; return *this; }
        layerBuilder withSelectionController() { _withSelectionController = true; return *this; }
        layerBuilder withTranslationController() { _withTranslationController = true; return *this; }
        layerBuilder withRotationController() { _withRotationController = true; return *this; }
        layerBuilder withUIMouseController() { _withUIMouseController = true; return *this; }

        LAYERS_API layer* build();
    };
}
