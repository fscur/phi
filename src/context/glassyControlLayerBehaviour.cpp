#include <precompiled.h>
#include "glassyControlLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    glassyControlLayerBehaviour::glassyControlLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _descriptor(new glassyControlRendererDescriptor(resolution)),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = glassyControlRenderer::configure(
            _descriptor,
            resolution, 
            resourcesPath, 
            framebufferAllocator);
    }

    glassyControlLayerBehaviour::~glassyControlLayerBehaviour()
    {
        safeDelete(_descriptor);
    }

    void glassyControlLayerBehaviour::onNodeAdded(node* node)
    {
        auto control = node->getComponent<phi::control>();
        if (control)
        {
            _descriptor->add(control);
        }
    }

    void glassyControlLayerBehaviour::onNodeRemoved(node* node)
    {
        auto control = node->getComponent<phi::control>();
        if (control)
        {
            _descriptor->remove(control);
        }
    }

    void glassyControlLayerBehaviour::onNodeTransformChanged(node* node)
    {
        throw notImplementedException();
    }

    void glassyControlLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        throw notImplementedException();
    }
}