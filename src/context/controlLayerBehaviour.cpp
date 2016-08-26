#include <precompiled.h>
#include "controlLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    controlLayerBehaviour::controlLayerBehaviour(
        const resolution & resolution, 
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new controlRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = controlRenderer::configure(_adapter, resolution, resourcesPath, framebufferAllocator);
    }

    controlLayerBehaviour::~controlLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void controlLayerBehaviour::onNodeAdded(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
        {
            control->colorChanged.assign(std::bind(&controlLayerBehaviour::onControlColorChanged, this, std::placeholders::_1));

            _adapter->add(control);
        }
    }

    void controlLayerBehaviour::onNodeRemoved(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _adapter->remove(control);
    }

    void controlLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _adapter->update(control);
    }

    void controlLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        throw notImplementedException();
    }

    void controlLayerBehaviour::onControlColorChanged(control* control)
    {
        _adapter->update(control);
    }
}