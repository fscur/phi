#include <precompiled.h>

#include <core/resolution.h>

#include <io/path.h>

#include <sceneRendering/rotationPlaneGridLayerBehaviour.h>

#include <ui/control.h>
#include <ui/text.h>

#include "ui/onDemandUiLayerBehaviour.h"
#include "ui/uiMouseController.h"

#include "layerBuilder.h"

namespace phi
{
    layerBuilder::layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager) :
        _layer(layer),
        _resolution(resolution),
        _resourcesPath(resourcesPath),
        _framebufferAllocator(framebufferAllocator),
        _meshBehaviour(nullptr),
        _physicsBehaviour(nullptr),
        _selectionBehaviour(nullptr),
        _onDemandUiTargetLayer(nullptr),
        _commandsManager(commandsManager),
        _withMeshRenderer(false),
        _withGhostMeshRenderer(false),
        _withObbRenderer(false),
        _withBoxColliderRenderer(false),
        _withTranslationPlaneGridRenderer(false),
        _withRotationPlaneGridRenderer(false),
        _withControlRenderer(false),
        _withTextRenderer(false),
        _withOnDemandUi(false),
        _withPhysics(false),
        _withCameraController(false),
        _withSelectionController(false),
        _withTranslationController(false),
        _withRotationController(false),
        _withUIMouseController(false)
    {
    }

    layerBuilder layerBuilder::newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager)
    {
        return layerBuilder(new layer(camera), camera->getResolution(), resourcesPath, framebufferAllocator, commandsManager);
    }

    layerBuilder layerBuilder::withOnDemandUi(layer * targetLayer, std::function<node*()> createUiFunction)
    {
        _withOnDemandUi = true;
        _onDemandUiTargetLayer = targetLayer;
        _onDemandUiCreateUiFunction = createUiFunction;
        return *this;
    }

    void layerBuilder::buildMeshRenderer()
    {
        auto meshBehaviour = new meshLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&meshLayerBehaviour::onNodeAdded, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&meshLayerBehaviour::onNodeRemoved, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&meshLayerBehaviour::onNodeTransformChanged, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&meshLayerBehaviour::onNodeSelectionChanged, meshBehaviour, std::placeholders::_1));
        _layer->addRenderPasses(meshBehaviour->getRenderPasses());

        _layer->addOnDelete([meshBehaviour]() mutable
        {
            safeDelete(meshBehaviour);
        });

        _meshBehaviour = meshBehaviour;
    }

    void layerBuilder::buildGhostMeshRenderer()
    {
        auto ghostMeshBehaviour = new ghostMeshLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&ghostMeshLayerBehaviour::onNodeAdded, ghostMeshBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&ghostMeshLayerBehaviour::onNodeRemoved, ghostMeshBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&ghostMeshLayerBehaviour::onNodeTransformChanged, ghostMeshBehaviour, std::placeholders::_1));
        _layer->addRenderPasses(ghostMeshBehaviour->getRenderPasses());

        _layer->addOnDelete([ghostMeshBehaviour]() mutable
        {
            safeDelete(ghostMeshBehaviour);
        });

        _ghostMeshBehaviour = ghostMeshBehaviour;
    }

    void layerBuilder::buildObbRenderer()
    {
        auto obbBehaviour = new obbLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&obbLayerBehaviour::onNodeAdded, obbBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&obbLayerBehaviour::onNodeRemoved, obbBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&obbLayerBehaviour::onNodeTransformChanged, obbBehaviour, std::placeholders::_1));
        _layer->addOnNodeObbChanged(std::bind(&obbLayerBehaviour::onNodeObbChanged, obbBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(obbBehaviour->getRenderPasses());

        _layer->addOnDelete([obbBehaviour]() mutable
        {
            safeDelete(obbBehaviour);
        });
    }

    void layerBuilder::buildBoxColliderRenderer()
    {
        auto boxColliderBehaviour = new boxColliderLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&boxColliderLayerBehaviour::onNodeAdded, boxColliderBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&boxColliderLayerBehaviour::onNodeRemoved, boxColliderBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&boxColliderLayerBehaviour::onNodeTransformChanged, boxColliderBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(boxColliderBehaviour->getRenderPasses());

        _layer->addOnDelete([boxColliderBehaviour]() mutable
        {
            safeDelete(boxColliderBehaviour);
        });
    }

    void layerBuilder::buildTranslationPlaneGridRenderer()
    {
        auto planeGridBehaviour = new translationPlaneGridLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&translationPlaneGridLayerBehaviour::onNodeAdded, planeGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&translationPlaneGridLayerBehaviour::onNodeRemoved, planeGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&translationPlaneGridLayerBehaviour::onNodeTransformChanged, planeGridBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(planeGridBehaviour->getRenderPasses());

        _layer->addOnDelete([planeGridBehaviour]() mutable
        {
            safeDelete(planeGridBehaviour);
        });
    }

    void layerBuilder::buildRotationPlaneGridRenderer()
    {
        auto rotationPlaneGridBehaviour = new rotationPlaneGridLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&rotationPlaneGridLayerBehaviour::onNodeAdded, rotationPlaneGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&rotationPlaneGridLayerBehaviour::onNodeRemoved, rotationPlaneGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&rotationPlaneGridLayerBehaviour::onNodeTransformChanged, rotationPlaneGridBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(rotationPlaneGridBehaviour->getRenderPasses());

        _layer->addOnDelete([rotationPlaneGridBehaviour]() mutable
        {
            safeDelete(rotationPlaneGridBehaviour);
        });
    }

    void layerBuilder::buildControlRenderer()
    {
        auto adapter = new controlRenderAdapter(_layer->getCamera());
        auto renderPasses = controlRenderer::configure(adapter, _resolution, _resourcesPath, _framebufferAllocator);
        auto controlBehaviour = new controlLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator, adapter, renderPasses);

        _layer->addOnNodeAdded(std::bind(&controlLayerBehaviour::onNodeAdded, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&controlLayerBehaviour::onNodeRemoved, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&controlLayerBehaviour::onNodeTransformChanged, controlBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(controlBehaviour->getRenderPasses());

        _layer->addOnDelete([controlBehaviour]() mutable
        {
            safeDelete(controlBehaviour);
        });
    }

    void layerBuilder::buildTextRenderer()
    {
        auto adapter = new textRenderAdapter(_layer->getCamera());
        auto textBehaviour = new textLayerBehaviour(
            _resolution,
            _resourcesPath,
            _framebufferAllocator,
            adapter);

        _layer->addOnNodeAdded(std::bind(&textLayerBehaviour::onNodeAdded, textBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&textLayerBehaviour::onNodeRemoved, textBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&textLayerBehaviour::onNodeTransformChanged, textBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&textLayerBehaviour::onNodeSelectionChanged, textBehaviour, std::placeholders::_1));
        _layer->addRenderPasses(textBehaviour->getRenderPasses());

        _layer->addOnDelete([textBehaviour]() mutable
        {
            safeDelete(textBehaviour);
        });
    }

    void layerBuilder::buildOnDemandUi()
    {
        auto onDemandUiBehaviour = new onDemandUiLayerBehaviour(_layer, _onDemandUiTargetLayer, _onDemandUiCreateUiFunction);
        _layer->addOnDelete([onDemandUiBehaviour]() mutable
        {
            safeDelete(onDemandUiBehaviour);
        });
    }

    void layerBuilder::buildPhysics()
    {
        auto physicsBehaviour = new physicsLayerBehaviour();

        _layer->addOnNodeAdded(std::bind(&physicsLayerBehaviour::onNodeAdded, physicsBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&physicsLayerBehaviour::onNodeRemoved, physicsBehaviour, std::placeholders::_1));
        _layer->addOnDelete([physicsBehaviour]() mutable
        {
            safeDelete(physicsBehaviour);
        });

        _physicsBehaviour = physicsBehaviour;
    }

    void layerBuilder::buildAnimation()
    {
        auto animatorBehaviour = new animatorLayerBehaviour();

        _layer->addOnUpdate(std::bind(&animatorLayerBehaviour::onUpdate, animatorBehaviour));
        _layer->addOnNodeAdded(std::bind(&animatorLayerBehaviour::onNodeAdded, animatorBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&animatorLayerBehaviour::onNodeRemoved, animatorBehaviour, std::placeholders::_1));

        _layer->addOnDelete([animatorBehaviour]() mutable
        {
            safeDelete(animatorBehaviour);
        });
    }

    void layerBuilder::buildCameraController()
    {
        _layer->addMouseController(new cameraInputController(_layer->getCamera()));
    }

    void layerBuilder::buildSelectionController()
    {
        auto selectionBehaviour = new selectionLayerBehaviour();

        _layer->addOnNodeSelectionChanged(std::bind(&selectionLayerBehaviour::onNodeSelectionChanged, selectionBehaviour, std::placeholders::_1));
        _layer->addOnDelete([selectionBehaviour]() mutable
        {
            safeDelete(selectionBehaviour);
        });

        _selectionBehaviour= selectionBehaviour;
        _layer->addMouseController(new selectionInputController(_commandsManager, selectionBehaviour));
    }

    void layerBuilder::buildTranslationController()
    {
        if (!_withSelectionController)
            throw invalidLayerConfigurationException("Translation Controller could not be added. It requires Selection Controller.");

        auto translationController = new translationInputController(
            _commandsManager,
            _selectionBehaviour->getSelectedNodes(), 
            _layer,
            _physicsBehaviour->getPhysicsWorld());

        _layer->addMouseController(translationController);
    }

    void layerBuilder::buildRotationController()
    {
        if (!_withRotationController)
            throw invalidLayerConfigurationException("Rotation Controller could not be added. It requires Selection Controller.");

        auto rotationController = new rotationInputController(
            _commandsManager,
            _selectionBehaviour->getSelectedNodes(),
            _layer,
            _physicsBehaviour->getPhysicsWorld());

        _layer->addMouseController(rotationController);
    }

    void layerBuilder::buildUIMouseController()
    {
        auto controller = new uiMouseController(_layer);
        _layer->addMouseController(controller);
    }

    layer* layerBuilder::build()
    {
        if (_withMeshRenderer)
            buildMeshRenderer();

        if (_withGhostMeshRenderer)
            buildGhostMeshRenderer();

        if (_withObbRenderer)
            buildObbRenderer();

        if (_withBoxColliderRenderer)
            buildBoxColliderRenderer();

        if (_withTranslationPlaneGridRenderer)
            buildTranslationPlaneGridRenderer();

        if (_withRotationPlaneGridRenderer)
            buildRotationPlaneGridRenderer();

        if (_withControlRenderer)
            buildControlRenderer();

        if (_withTextRenderer)
            buildTextRenderer();

        if (_withOnDemandUi)
            buildOnDemandUi();

        if (_withPhysics)
            buildPhysics();

        if (_withAnimation)
            buildAnimation();

        if (_withCameraController)
            buildCameraController();

        if (_withSelectionController)
            buildSelectionController();

        if (_withTranslationController)
            buildTranslationController();

        if (_withRotationController)
            buildRotationController();

        if (_withUIMouseController)
            buildUIMouseController();

        return _layer;
    }
}