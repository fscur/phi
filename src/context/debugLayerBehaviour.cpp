#include <precompiled.h>
#include "debugLayerBehaviour.h"

#include <core\notImplementedException.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>
#include <debugRendering\boxColliderRenderPass.h>
#include <core\mesh.h>

namespace phi
{
    debugLayerBehaviour::debugLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new boxColliderRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto boxRenderPass = boxColliderRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { boxRenderPass };
    }

    debugLayerBehaviour::~debugLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void debugLayerBehaviour::onNodeAdded(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _adapter->add(boxCollider);
    }

    void debugLayerBehaviour::onNodeRemoved(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _adapter->remove(boxCollider);
    }

    void debugLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _adapter->update(boxCollider);
    }

    void debugLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        throw notImplementedException();
    }
}