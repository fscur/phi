#include <precompiled.h>
#include "glassyControlLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    glassyControlLayerBehaviour::glassyControlLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new controlRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = glassyControlRenderer::configure(
            _adapter,
            resolution, 
            resourcesPath, 
            framebufferAllocator);
    }

    glassyControlLayerBehaviour::~glassyControlLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void glassyControlLayerBehaviour::onNodeAdded(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _adapter->add(control);
    }

    void glassyControlLayerBehaviour::onNodeRemoved(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _adapter->remove(control);
    }

    void glassyControlLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _adapter->update(control);
    }

    void glassyControlLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        throw notImplementedException();
    }
}