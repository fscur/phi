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

#include "meshLayerBehaviour.h"
#include "controlLayerBehaviour.h"
#include "glassyControlLayerBehaviour.h"
#include "textLayerBehaviour.h"
#include "cameraInputController.h"
#include "selectionInputController.h"
#include "obbDragInputController.h"

namespace phi
{
    layerBuilder::layerBuilder(layer* layer, resolution resolution, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager) :
        _layer(layer),
        _resolution(resolution),
        _resourcesPath(resourcesPath),
        _framebufferAllocator(framebufferAllocator),
        _commandsManager(commandsManager)
    {
    }

    layerBuilder layerBuilder::newLayer(camera* camera, string resourcesPath, framebufferAllocator* framebufferAllocator, commandsManager* commandsManager)
    {
        return layerBuilder(new layer(camera), camera->getResolution(), resourcesPath, framebufferAllocator, commandsManager);
    }

    layerBuilder layerBuilder::withMeshRenderer()
    {
        auto meshBehaviour = new meshLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&meshLayerBehaviour::onNodeAdded, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&meshLayerBehaviour::onNodeRemoved, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&meshLayerBehaviour::onNodeTransformChanged, meshBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&meshLayerBehaviour::onNodeSelectionChanged, meshBehaviour, std::placeholders::_1));
        _layer->addRenderPasses(meshBehaviour->getRenderPasses());

        _layer->addOnDelete([meshBehaviour] () mutable
        {
            safeDelete(meshBehaviour);
        });

        _layer->addMouseController(new cameraInputController(_layer->getCamera()));
        _layer->addMouseController(new selectionInputController(meshBehaviour, _commandsManager));
        _layer->addMouseController(new obbDragInputController(_layer->getCamera()));

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

        return *this;
    }

    layerBuilder layerBuilder::withGlassyControlRenderer()
    {
        auto glassyBehaviour = new glassyControlLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&glassyControlLayerBehaviour::onNodeAdded, glassyBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&glassyControlLayerBehaviour::onNodeRemoved, glassyBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&glassyControlLayerBehaviour::onNodeTransformChanged, glassyBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&glassyControlLayerBehaviour::onNodeSelectionChanged, glassyBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(glassyBehaviour->getRenderPasses());

        _layer->addOnDelete([glassyBehaviour] () mutable
        {
            safeDelete(glassyBehaviour);
        });

        return *this;
    }

    layerBuilder layerBuilder::withControlRenderer()
    {
        auto controlBehaviour = new controlLayerBehaviour(_resolution, _resourcesPath, _framebufferAllocator);

        _layer->addOnNodeAdded(std::bind(&controlLayerBehaviour::onNodeAdded, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeRemoved(std::bind(&controlLayerBehaviour::onNodeRemoved, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeTransformChanged(std::bind(&controlLayerBehaviour::onNodeTransformChanged, controlBehaviour, std::placeholders::_1));
        _layer->addOnNodeSelectionChanged(std::bind(&controlLayerBehaviour::onNodeSelectionChanged, controlBehaviour, std::placeholders::_1));

        _layer->addRenderPasses(controlBehaviour->getRenderPasses());

        _layer->addOnDelete([controlBehaviour] () mutable
        {
            safeDelete(controlBehaviour);
        });

        return *this;
    }

    layerBuilder layerBuilder::withTextRenderer()
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

        _layer->addOnDelete([textBehaviour] () mutable
        {
            safeDelete(textBehaviour);
        });

        return *this;
    }

    layer* layerBuilder::build()
    {
        return _layer;
    }
}