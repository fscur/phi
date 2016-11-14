#include <precompiled.h>

#include <core/resolution.h>

#include <io/path.h>

#include "layerBuilder.h"

namespace phi
{
    layerBuilder::layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager) :
        _layer(layer),
        _resolution(resolution),
        _resourcesPath(resourcesPath),
        _framebufferAllocator(framebufferAllocator),
        meshLayerBehaviour(nullptr),
        ghostMeshLayerBehaviour(nullptr),
        obbLayerBehaviour(nullptr),
        boxColliderLayerBehaviour(nullptr),
        translationPlaneGridLayerBehaviour(nullptr),
        rotationPlaneGridLayerBehaviour(nullptr),
        controlLayerBehaviour(nullptr),
        textLayerBehaviour(nullptr),
        physicsLayerBehaviour(nullptr),
        animatorLayerBehaviour(nullptr),
        cameraInputController(nullptr),
        selectionLayerBehaviour(nullptr),
        selectionInputController(nullptr),
        translationInputController(nullptr),
        rotationInputController(nullptr),
        uiInputController(nullptr),
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

    void layerBuilder::buildMeshRenderer()
    {
        auto meshBehaviour = new phi::meshLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&meshLayerBehaviour::onNodeAdded, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&meshLayerBehaviour::onNodeRemoved, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&meshLayerBehaviour::onNodeTransformChanged, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&meshLayerBehaviour::onNodeSelectionChanged, meshBehaviour, std::placeholders::_1));
        _layer->addRenderPasses(meshBehaviour->getRenderPasses());

        _layer->addOnDelete([meshBehaviour]() mutable
        {
            safeDelete(meshBehaviour);
        });

        meshLayerBehaviour = meshBehaviour;
    }

    void layerBuilder::buildGhostMeshRenderer()
    {
        auto ghostMeshBehaviour = new phi::ghostMeshLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&ghostMeshLayerBehaviour::onNodeAdded, ghostMeshBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&ghostMeshLayerBehaviour::onNodeRemoved, ghostMeshBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&ghostMeshLayerBehaviour::onNodeTransformChanged, ghostMeshBehaviour, std::placeholders::_1));
        _layer->addRenderPasses(ghostMeshBehaviour->getRenderPasses());

        _layer->addOnDelete([ghostMeshBehaviour]() mutable
        {
            safeDelete(ghostMeshBehaviour);
        });

        ghostMeshLayerBehaviour = ghostMeshBehaviour;
    }

    void layerBuilder::buildObbRenderer()
    {
        auto obbBehaviour = new phi::obbLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&obbLayerBehaviour::onNodeAdded, obbBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&obbLayerBehaviour::onNodeRemoved, obbBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&obbLayerBehaviour::onNodeTransformChanged, obbBehaviour, std::placeholders::_1));
        _layer->addOnNodeObbChanged(std::bind(&obbLayerBehaviour::onNodeObbChanged, obbBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(obbBehaviour->getRenderPasses());

        _layer->addOnDelete([obbBehaviour]() mutable
        {
            safeDelete(obbBehaviour);
        });

        obbLayerBehaviour = obbBehaviour;
    }

    void layerBuilder::buildBoxColliderRenderer()
    {
        auto boxColliderBehaviour = new phi::boxColliderLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&boxColliderLayerBehaviour::onNodeAdded, boxColliderBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&boxColliderLayerBehaviour::onNodeRemoved, boxColliderBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&boxColliderLayerBehaviour::onNodeTransformChanged, boxColliderBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(boxColliderBehaviour->getRenderPasses());

        _layer->addOnDelete([boxColliderBehaviour]() mutable
        {
            safeDelete(boxColliderBehaviour);
        });

        boxColliderLayerBehaviour = boxColliderBehaviour;
    }

    void layerBuilder::buildTranslationPlaneGridRenderer()
    {
        auto translationPlaneGridBehaviour = new phi::translationPlaneGridLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&translationPlaneGridLayerBehaviour::onNodeAdded, translationPlaneGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&translationPlaneGridLayerBehaviour::onNodeRemoved, translationPlaneGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&translationPlaneGridLayerBehaviour::onNodeTransformChanged, translationPlaneGridBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(translationPlaneGridBehaviour->getRenderPasses());

        _layer->addOnDelete([translationPlaneGridBehaviour]() mutable
        {
            safeDelete(translationPlaneGridBehaviour);
        });

        translationPlaneGridLayerBehaviour = translationPlaneGridBehaviour;
    }

    void layerBuilder::buildRotationPlaneGridRenderer()
    {
        auto rotationPlaneGridBehaviour = new phi::rotationPlaneGridLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&rotationPlaneGridLayerBehaviour::onNodeAdded, rotationPlaneGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&rotationPlaneGridLayerBehaviour::onNodeRemoved, rotationPlaneGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&rotationPlaneGridLayerBehaviour::onNodeTransformChanged, rotationPlaneGridBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(rotationPlaneGridBehaviour->getRenderPasses());

        _layer->addOnDelete([rotationPlaneGridBehaviour]() mutable
        {
            safeDelete(rotationPlaneGridBehaviour);
        });

        rotationPlaneGridLayerBehaviour = rotationPlaneGridBehaviour;
    }

    void layerBuilder::buildControlRenderer()
    {
        auto adapter = new controlRenderAdapter(_layer->getCamera());
        auto renderPasses = controlRenderer::configure(adapter, _resolution, _resourcesPath, _framebufferAllocator);
        auto controlBehaviour = new phi::controlLayerBehaviour(_resolution, _resourcesPath, adapter, renderPasses);

        _layer->addOnNodeAdded(std::bind(&controlLayerBehaviour::onNodeAdded, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&controlLayerBehaviour::onNodeRemoved, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&controlLayerBehaviour::onNodeTransformChanged, controlBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(controlBehaviour->getRenderPasses());

        _layer->addOnDelete([controlBehaviour]() mutable
        {
            safeDelete(controlBehaviour);
        });

        controlLayerBehaviour = controlBehaviour;
    }

    void layerBuilder::buildTextRenderer()
    {
        auto adapter = new textRenderAdapter(_layer->getCamera());
        auto textBehaviour = new phi::textLayerBehaviour(
            adapter,
            _resolution,
            _resourcesPath);

        _layer->addOnNodeAdded(std::bind(&textLayerBehaviour::onNodeAdded, textBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&textLayerBehaviour::onNodeRemoved, textBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&textLayerBehaviour::onNodeTransformChanged, textBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&textLayerBehaviour::onNodeSelectionChanged, textBehaviour, std::placeholders::_1));
        _layer->addRenderPasses(textBehaviour->getRenderPasses());

        _layer->addOnDelete([textBehaviour]() mutable
        {
            safeDelete(textBehaviour);
        });

        textLayerBehaviour = textBehaviour;
    }

    void layerBuilder::buildPhysics()
    {
        auto physicsBehaviour = new phi::physicsLayerBehaviour();

        _layer->addOnNodeAdded(std::bind(&physicsLayerBehaviour::onNodeAdded, physicsBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&physicsLayerBehaviour::onNodeRemoved, physicsBehaviour, std::placeholders::_1));
        _layer->addOnDelete([physicsBehaviour]() mutable
        {
            safeDelete(physicsBehaviour);
        });

        physicsLayerBehaviour = physicsBehaviour;
    }

    void layerBuilder::buildAnimation()
    {
        auto animatorBehaviour = new phi::animatorLayerBehaviour();

        _layer->addOnUpdate(std::bind(&animatorLayerBehaviour::onUpdate, animatorBehaviour));
        _layer->addOnNodeAdded(std::bind(&animatorLayerBehaviour::onNodeAdded, animatorBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&animatorLayerBehaviour::onNodeRemoved, animatorBehaviour, std::placeholders::_1));
        _layer->addOnNodeComponentAdded(std::bind(&animatorLayerBehaviour::onNodeComponentAdded, animatorBehaviour, std::placeholders::_1, std::placeholders::_2));
        _layer->addOnNodeComponentRemoved(std::bind(&animatorLayerBehaviour::onNodeComponentRemoved, animatorBehaviour, std::placeholders::_1, std::placeholders::_2));

        _layer->addOnDelete([animatorBehaviour]() mutable
        {
            safeDelete(animatorBehaviour);
        });

        animatorLayerBehaviour = animatorBehaviour;
    }

    void layerBuilder::buildCameraController()
    {
        cameraInputController = new phi::cameraInputController(_layer->getCamera());
        _layer->addInputController(cameraInputController);
    }

    void layerBuilder::buildSelectionController()
    {
        auto selectionBehaviour = new phi::selectionLayerBehaviour();

        _layer->addOnNodeSelectionChanged(std::bind(&selectionLayerBehaviour::onNodeSelectionChanged, selectionBehaviour, std::placeholders::_1));
        _layer->addOnDelete([selectionBehaviour]() mutable
        {
            safeDelete(selectionBehaviour);
        });

        selectionLayerBehaviour= selectionBehaviour;

        selectionInputController = new phi::selectionInputController(_commandsManager, selectionBehaviour);
        _layer->addInputController(selectionInputController);

        selectionLayerBehaviour = selectionBehaviour;
    }

    void layerBuilder::buildTranslationController()
    {
        if (!_withSelectionController)
            throw invalidLayerConfigurationException("Translation Controller could not be added. It requires Selection Controller.");

        translationInputController = new phi::translationInputController(
            _commandsManager,
            selectionLayerBehaviour->getSelectedNodes(), 
            _layer,
            physicsLayerBehaviour->getPhysicsWorld());

        _layer->addInputController(translationInputController);
    }

    void layerBuilder::buildRotationController()
    {
        if (!_withRotationController)
            throw invalidLayerConfigurationException("Rotation Controller could not be added. It requires Selection Controller.");

        rotationInputController = new phi::rotationInputController(
            _commandsManager,
            selectionLayerBehaviour->getSelectedNodes(),
            _layer,
            physicsLayerBehaviour->getPhysicsWorld());

        _layer->addInputController(rotationInputController);
    }

    void layerBuilder::buildUIMouseController()
    {
        uiInputController = new phi::uiInputController(_layer);
        _layer->addInputController(uiInputController);
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