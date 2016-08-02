#include <precompiled.h>
#include "layerBuilder.h"

#include <core\resolution.h>

#include <ui\control.h>
#include <ui\text.h>

#include <io\path.h>

#include <rendering\camera.h>
#include <rendering\framebufferAllocator.h>
#include <sceneRendering\meshRenderer.h>

#include <uiRendering\controlRenderer.h>
#include <uiRendering\glassyControlRenderer.h>
#include <uiRendering\textRenderer.h>

#include "invalidLayerConfigurationException.h"
#include "meshLayerBehaviour.h"
#include "boxColliderLayerBehaviour.h"
#include "planeGridLayerBehaviour.h"
#include "controlLayerBehaviour.h"
#include "glassyControlLayerBehaviour.h"
#include "textLayerBehaviour.h"
#include "physicsLayerBehaviour.h"
#include "animatorLayerBehaviour.h"

#include "cameraInputController.h"
#include "selectionInputController.h"
#include "translationInputController.h"
#include "planesTranslationInputController.h"

namespace phi
{
    layerBuilder::layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager) :
        _layer(layer),
        _resolution(resolution),
        _resourcesPath(resourcesPath),
        _framebufferAllocator(framebufferAllocator),
        _meshBehaviour(nullptr),
        _physicsBehaviour(nullptr),
        _commandsManager(commandsManager),
        _withMeshRenderer(false),
        _withBoxColliderRenderer(false),
        _withPlaneGridRenderer(false),
        _withControlRenderer(false),
        _withGlassyControlRenderer(false),
        _withTextRenderer(false),
        _withPhysics(false),
        _withCameraController(false),
        _withSelectionController(false),
        _withTranslationController(false),
        _withPlanesTranslationController(false)
    {
    }

    layerBuilder layerBuilder::newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager)
    {
        return layerBuilder(new layer(camera), camera->getResolution(), resourcesPath, framebufferAllocator, commandsManager);
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

        // Camera:
            // Pan
            // Rotate
            // Zoom
        // Objetos:
            // Translate
            // Rotate
            // Selection
        // UI:
            // Drag and drop
            // Resto
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

    void layerBuilder::buildPlaneGridRenderer()
    {
        auto planeGridBehaviour = new planeGridLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);
        _layer->addOnNodeAdded(std::bind(&planeGridLayerBehaviour::onNodeAdded, planeGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&planeGridLayerBehaviour::onNodeRemoved, planeGridBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&planeGridLayerBehaviour::onNodeTransformChanged, planeGridBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(planeGridBehaviour->getRenderPasses());

        _layer->addOnDelete([planeGridBehaviour]() mutable
        {
            safeDelete(planeGridBehaviour);
        });
    }

    void layerBuilder::buildGlassyControlRenderer()
    {
        auto glassyBehaviour = new glassyControlLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&glassyControlLayerBehaviour::onNodeAdded, glassyBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&glassyControlLayerBehaviour::onNodeRemoved, glassyBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&glassyControlLayerBehaviour::onNodeTransformChanged, glassyBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&glassyControlLayerBehaviour::onNodeSelectionChanged, glassyBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(glassyBehaviour->getRenderPasses());

        _layer->addOnDelete([glassyBehaviour]() mutable
        {
            safeDelete(glassyBehaviour);
        });
    }

    void layerBuilder::buildControlRenderer()
    {
        auto controlBehaviour = new controlLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&controlLayerBehaviour::onNodeAdded, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&controlLayerBehaviour::onNodeRemoved, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&controlLayerBehaviour::onNodeTransformChanged, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&controlLayerBehaviour::onNodeSelectionChanged, controlBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(controlBehaviour->getRenderPasses());

        _layer->addOnDelete([controlBehaviour]() mutable
        {
            safeDelete(controlBehaviour);
        });
    }

    void layerBuilder::buildTextRenderer()
    {
        auto textBehaviour = new textLayerBehaviour(
            _resolution,
            _resourcesPath,
            _framebufferAllocator);

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
        if (!_withMeshRenderer)
            throw invalidLayerConfigurationException("Selection Controller could not be added. It requires a Mesh Renderer.");

        _layer->addMouseController(new selectionInputController(_meshBehaviour, _commandsManager));
    }

    void layerBuilder::buildTranslationController()
    {
        auto obbTranslationController = new translationInputController(_layer->getCamera(), _layer);
        if (_withPhysics)
            obbTranslationController->setCollisionNodeTranslator(new collisionNodeTranslator(_physicsBehaviour->getPhysicsWorld()));

        _layer->addMouseController(obbTranslationController);
    }

    void layerBuilder::buildPlanesTranslationController()
    {
        if (!_withPhysics)
            throw invalidLayerConfigurationException("Planes Translation Controller could not be added. It requires Physics.");

        auto planesTranslationController = new planesTranslationInputController(_layer->getCamera(), _layer, _physicsBehaviour);
        planesTranslationController->setCollisionNodeTranslator(new collisionNodeTranslator(_physicsBehaviour->getPhysicsWorld()));

        _layer->addMouseController(planesTranslationController);
    }

    layer* layerBuilder::build()
    {
        if (_withMeshRenderer)
            buildMeshRenderer();

        if (_withBoxColliderRenderer)
            buildBoxColliderRenderer();

        if (_withPlaneGridRenderer)
            buildPlaneGridRenderer();

        if (_withGlassyControlRenderer)
            buildGlassyControlRenderer();

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

        if (_withPlanesTranslationController)
            buildPlanesTranslationController();

        return _layer;
    }
}